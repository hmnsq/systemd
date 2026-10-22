/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdlib.h>
#ضمن <sys/stat.h>
#ضمن <sys/uio.h>
#ضمن <time.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "chattr-util.h"
#ضمن "efivars.h"
#ضمن "fd-util.h"
#ضمن "io-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "time-util.h"
#ضمن "utf8.h"
#ضمن "virt.h"

#אם ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

صحيح efi_get_variable(
                ثابت حرف *variable,
                uint32_t *ret_attribute,
                فراغ **ret_value,
                size_t *ret_size) {

        صحيح r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        أكد(variable);

        ثابت حرف *p = strjoina("/sys/firmware/efi/efivars/", variable);

        אם (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ صحيح fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        אם (fd < 0)
                החזר log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ حرف *buf = عدم;
        ssize_t n;

        /* The kernel ratelimits reads from the efivarfs because EFI is inefficient, and we'll occasionally
         * fail with EINTR here. A slowdown is better than a failure for us, so retry a few times and
         * eventually fail with -EBUSY.
         *
         * See https://github.com/torvalds/linux/blob/master/fs/efivarfs/file.c#L75 and
         * https://github.com/torvalds/linux/commit/bef3efbeb897b56867e271cdbc5f8adaacaeb9cd.
         *
         * The variable may also be overwritten between the stat and read. If we find out that the new
         * contents are longer, try again.
         */
        עבור (unsigned try = 0;; try++) {
                بنية stat st;

                אם (fstat(fd, &st) < 0)
                        החזר log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                אם (r < 0)
                        החזר log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                אם (st.st_size == 0) /* for uncommitted variables, see below */
                        החזר log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                אם ((uint64_t) st.st_size < حجم(attr))
                        החזר log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, حجم(attr));
                אם ((uint64_t) st.st_size > حجم(attr) + 4 * U64_MB)
                        החזר log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                אם (!ret_attribute && !ret_value) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        اكسر;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                אם (!SUB_SAFE(&payload_size, file_size, حجم(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        החזר log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                حرر(buf);
                buf = malloc(alloc_size);
                אם (!buf)
                        החזר -ENOMEM;

                بنية iovec iov[] = {
                        { &attr, حجم(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                אם (n < 0) {
                        אם (errno != EINTR)
                                החזר log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } אחרת אם ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                אחרת {
                        أكد((size_t) n < read_limit);
                        اكسر;
                }

                אם (try >= EFI_N_RETRIES_TOTAL)
                        החזר log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                אם (try >= EFI_N_RETRIES_NO_DELAY)
                        (فراغ) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (فراغ) lseek(fd, 0, SEEK_SET);
        }

        /* Unfortunately kernel reports EOF if there's an inconsistency between efivarfs var list and
         * what's actually stored in firmware, c.f. #34304. A zero size env var is not allowed in EFI
         * and hence the variable doesn't really exist in the backing store as long as it is zero
         * sized, and the kernel calls this "uncommitted". Hence we translate EOF back to ENOENT
         * here, as with kernel behavior before
         * https://github.com/torvalds/linux/commit/3fab70c165795431f00ddf9be8b84ddd07bd1f8f.
         *
         * If the kernel changes behaviour (to flush dentries on resume), we can drop this at some
         * point in the future. But note that the commit is 11 years old at this point so we'll need
         * to deal with the current behaviour for a long time.
         */
        אם (n == 0)
                החזר log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        אם ((size_t) n < حجم(attr))
                החזר log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, حجم(attr));
        size_t value_size = n - حجم(attr);

        אם (ret_attribute)
                *ret_attribute = attr;

        אם (ret_value) {
                أكد(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[value_size] = 0;
                buf[value_size + 1] = 0;
                buf[value_size + 2] = 0;
                *ret_value = TAKE_PTR(buf);
        }

        אם (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                אם (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        אם (ret_size)
                *ret_size = value_size;

        החזר 0;
}

صحيح efi_get_variable_string(ثابت حرف *variable, حرف **ret) {
        _cleanup_free_ فراغ *s = عدم, *x = عدم;
        size_t ss = 0;
        صحيح r;

        أكد(variable);

        r = efi_get_variable(variable, عدم, &s, &ss);
        אם (r < 0)
                החזר r;

        x = utf16_to_utf8(s, ss);
        אם (!x)
                החזר -ENOMEM;

        אם (ret)
                *ret = TAKE_PTR(x);

        החזר 0;
}

صحيح efi_get_variable_path(ثابت حرف *variable, حرف **ret) {
        صحيح r;

        أكد(variable);

        r = efi_get_variable_string(variable, ret);
        אם (r < 0)
                החזר r;

        אם (ret)
                efi_tilt_backslashes(*ret);

        החזר r;
}

ساكن صحيح efi_verify_variable(ثابت حرف *variable, uint32_t attr, ثابت فراغ *value, size_t size) {
        _cleanup_free_ فراغ *buf = عدم;
        size_t n;
        uint32_t a;
        صحيح r;

        أكد(variable);
        أكد(value || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        אם (r < 0)
                החזר r;

        החזר a == attr && memcmp_nn(buf, n, value, size) == 0;
}

صحيح efi_set_variable(ثابت حرف *variable, ثابت فراغ *value, size_t size) {
        ساكن ثابت uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ بنية var {
                uint32_t attr;
                حرف buf[];
        } _packed_ *buf = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        صحيح r;

        أكد(variable);
        أكد(value || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        אם (size > 0 && efi_verify_variable(variable, attr, value, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                החזר 0;
        }

        ثابت حرف *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ عدم,
                        /* flags= */ 0);
        אם (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        אם (size == 0) {
                אם (unlink(p) < 0) {
                        r = -errno;
                        اذهب finish;
                }

                החזר 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        אם (fd < 0) {
                r = -errno;
                اذهب finish;
        }

        buf = malloc(حجم(uint32_t) + size);
        אם (!buf) {
                r = -ENOMEM;
                اذهب finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, value, size);

        r = loop_write(fd, buf, حجم(uint32_t) + size);
        אם (r < 0)
                اذهب finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        אם (futimens(fd, /* times= */ عدم) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        אם (saved_flags_valid) {
                صحيح q;

                /* Restore the original flags field, just in case */
                אם (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                אחרת
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                אם (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        החזר r;
}

صحيح efi_set_variable_string(ثابت حرف *variable, ثابت حرف *value) {
        _cleanup_free_ char16_t *u16 = عدم;

        u16 = utf8_to_utf16(value, SIZE_MAX);
        אם (!u16)
                החזר -ENOMEM;

        החזר efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * حجم(char16_t));
}

ساكن صحيح cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        החזר (cache_efi_boot = b);
}

bool is_efi_boot(فراغ) {
        אם (cache_efi_boot >= 0)
                החזר cache_efi_boot;

        אם (detect_container() > 0)
                החזר (cache_efi_boot = false);

        אם (access("/sys/firmware/efi/", F_OK) < 0) {
                אם (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                החזר (cache_efi_boot = false);
        }

        החזר (cache_efi_boot = true);
}

ساكن صحيح read_flag(ثابت حرف *variable) {
        _cleanup_free_ فراغ *v = عدم;
        uint8_t b;
        size_t s;
        صحيح r;

        אם (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                החזר 0;

        r = efi_get_variable(variable, عدم, &v, &s);
        אם (r < 0)
                החזר r;

        אם (s != 1)
                החזר -EINVAL;

        b = *(uint8_t *)v;
        החזר !!b;
}

bool is_efi_secure_boot(فراغ) {
        ساكن صحيح cache = -1;
        صحيح r;

        אם (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                אם (r == -ENOENT)
                        cache = false;
                אחרת אם (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                אחרת
                        cache = r;
        }

        החזר cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(فراغ) {
        ساكن SecureBootMode cache = _SECURE_BOOT_INVALID;

        אם (cache != _SECURE_BOOT_INVALID)
                החזר cache;

        صحيح secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        אם (secure < 0) {
                אם (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                החזר (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        صحيح audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        صحيح deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        صحيح setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        صحيح moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        החזר (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

حرف *efi_tilt_backslashes(حرف *s) {
        החזר string_replace_char(s, '\\', '/');
}
