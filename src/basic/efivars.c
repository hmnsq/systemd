/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <stdlib.h>
#شامل <sys/stat.h>
#شامل <sys/uio.h>
#شامل <time.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "chattr-util.h"
#شامل "efivars.h"
#شامل "fd-util.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "time-util.h"
#شامل "utf8.h"
#شامل "virt.h"

#اگر ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

صحیح efi_get_variable(
                ثابت نویسه *variable,
                uint32_t *ret_attribute,
                تهی **ret_مقدار,
                size_t *ret_size) {

        صحیح r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        أكد(variable);

        ثابت نویسه *p = strjoina("/sys/firmware/efi/efivars/", variable);

        اگر (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ صحیح fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ نویسه *buf = پوچ;
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
        برای (unsigned try = 0;; try++) {
                ساختار stat st;

                اگر (fstat(fd, &st) < 0)
                        بازگشت log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                اگر (st.st_size == 0) /* for uncommitted variables, see below */
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                اگر ((uint64_t) st.st_size < اندازه(attr))
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, اندازه(attr));
                اگر ((uint64_t) st.st_size > اندازه(attr) + 4 * U64_MB)
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                اگر (!ret_attribute && !ret_مقدار) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        بشکن;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                اگر (!SUB_SAFE(&payload_size, file_size, اندازه(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                آزاد(buf);
                buf = malloc(alloc_size);
                اگر (!buf)
                        بازگشت -ENOMEM;

                ساختار iovec iov[] = {
                        { &attr, اندازه(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                اگر (n < 0) {
                        اگر (errno != EINTR)
                                بازگشت log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } وگرنه اگر ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                وگرنه {
                        أكد((size_t) n < read_limit);
                        بشکن;
                }

                اگر (try >= EFI_N_RETRIES_TOTAL)
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                اگر (try >= EFI_N_RETRIES_NO_DELAY)
                        (تهی) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (تهی) lseek(fd, 0, SEEK_SET);
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
        اگر (n == 0)
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        اگر ((size_t) n < اندازه(attr))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, اندازه(attr));
        size_t مقدار_size = n - اندازه(attr);

        اگر (ret_attribute)
                *ret_attribute = attr;

        اگر (ret_مقدار) {
                أكد(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[مقدار_size] = 0;
                buf[مقدار_size + 1] = 0;
                buf[مقدار_size + 2] = 0;
                *ret_مقدار = TAKE_PTR(buf);
        }

        اگر (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                اگر (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        اگر (ret_size)
                *ret_size = مقدار_size;

        بازگشت 0;
}

صحیح efi_get_variable_string(ثابت نویسه *variable, نویسه **ret) {
        _cleanup_free_ تهی *s = پوچ, *x = پوچ;
        size_t ss = 0;
        صحیح r;

        أكد(variable);

        r = efi_get_variable(variable, پوچ, &s, &ss);
        اگر (r < 0)
                بازگشت r;

        x = utf16_to_utf8(s, ss);
        اگر (!x)
                بازگشت -ENOMEM;

        اگر (ret)
                *ret = TAKE_PTR(x);

        بازگشت 0;
}

صحیح efi_get_variable_path(ثابت نویسه *variable, نویسه **ret) {
        صحیح r;

        أكد(variable);

        r = efi_get_variable_string(variable, ret);
        اگر (r < 0)
                بازگشت r;

        اگر (ret)
                efi_tilt_backslashes(*ret);

        بازگشت r;
}

ایستا صحیح efi_verify_variable(ثابت نویسه *variable, uint32_t attr, ثابت تهی *مقدار, size_t size) {
        _cleanup_free_ تهی *buf = پوچ;
        size_t n;
        uint32_t a;
        صحیح r;

        أكد(variable);
        أكد(مقدار || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        اگر (r < 0)
                بازگشت r;

        بازگشت a == attr && memcmp_nn(buf, n, مقدار, size) == 0;
}

صحیح efi_set_variable(ثابت نویسه *variable, ثابت تهی *مقدار, size_t size) {
        ایستا ثابت uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ساختار var {
                uint32_t attr;
                نویسه buf[];
        } _packed_ *buf = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        صحیح r;

        أكد(variable);
        أكد(مقدار || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        اگر (size > 0 && efi_verify_variable(variable, attr, مقدار, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                بازگشت 0;
        }

        ثابت نویسه *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ پوچ,
                        /* flags= */ 0);
        اگر (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        اگر (size == 0) {
                اگر (unlink(p) < 0) {
                        r = -errno;
                        برو finish;
                }

                بازگشت 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        اگر (fd < 0) {
                r = -errno;
                برو finish;
        }

        buf = malloc(اندازه(uint32_t) + size);
        اگر (!buf) {
                r = -ENOMEM;
                برو finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, مقدار, size);

        r = loop_write(fd, buf, اندازه(uint32_t) + size);
        اگر (r < 0)
                برو finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        اگر (futimens(fd, /* times= */ پوچ) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        اگر (saved_flags_valid) {
                صحیح q;

                /* Restore the original flags field, just in case */
                اگر (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                وگرنه
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                اگر (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        بازگشت r;
}

صحیح efi_set_variable_string(ثابت نویسه *variable, ثابت نویسه *مقدار) {
        _cleanup_free_ char16_t *u16 = پوچ;

        u16 = utf8_to_utf16(مقدار, SIZE_MAX);
        اگر (!u16)
                بازگشت -ENOMEM;

        بازگشت efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * اندازه(char16_t));
}

ایستا صحیح cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        بازگشت (cache_efi_boot = b);
}

bool is_efi_boot(تهی) {
        اگر (cache_efi_boot >= 0)
                بازگشت cache_efi_boot;

        اگر (detect_container() > 0)
                بازگشت (cache_efi_boot = false);

        اگر (access("/sys/firmware/efi/", F_OK) < 0) {
                اگر (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                بازگشت (cache_efi_boot = false);
        }

        بازگشت (cache_efi_boot = true);
}

ایستا صحیح read_flag(ثابت نویسه *variable) {
        _cleanup_free_ تهی *v = پوچ;
        uint8_t b;
        size_t s;
        صحیح r;

        اگر (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                بازگشت 0;

        r = efi_get_variable(variable, پوچ, &v, &s);
        اگر (r < 0)
                بازگشت r;

        اگر (s != 1)
                بازگشت -EINVAL;

        b = *(uint8_t *)v;
        بازگشت !!b;
}

bool is_efi_secure_boot(تهی) {
        ایستا صحیح cache = -1;
        صحیح r;

        اگر (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                اگر (r == -ENOENT)
                        cache = false;
                وگرنه اگر (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                وگرنه
                        cache = r;
        }

        بازگشت cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(تهی) {
        ایستا SecureBootMode cache = _SECURE_BOOT_INVALID;

        اگر (cache != _SECURE_BOOT_INVALID)
                بازگشت cache;

        صحیح secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        اگر (secure < 0) {
                اگر (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                بازگشت (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        صحیح audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        صحیح deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        صحیح setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        صحیح moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        بازگشت (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

نویسه *efi_tilt_backslashes(نویسه *s) {
        بازگشت string_replace_char(s, '\\', '/');
}
