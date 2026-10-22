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

#إذا ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#تعریف_کن EFI_N_RETRIES_NO_DELAY 20
#تعریف_کن EFI_N_RETRIES_مجموع 25
#تعریف_کن EFI_RETRY_DELAY (50 * USEC_در_هر_MSEC)

صحيح efi_get_variable(
                ثابت حرف *variable,
                uint32_t *ret_attribute,
                فراغ **ret_مقدار,
                size_t *ret_اندازه) {

        صحيح r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        أكد(variable);

        ثابت حرف *p = strjoina("/sys/firmware/efi/efivars/", variable);

        إذا (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ صحيح fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع log_debug_errno(errno, "open(\"%s\") failed: %m", p);

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
        لكل (بیعلامت try = 0;; try++) {
                بنية stat st;

                إذا (fstat(fd, &st) < 0)
                        أرجع log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                إذا (r < 0)
                        أرجع log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                إذا (st.st_size == 0) /* for uncommitted variables, see below */
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                إذا ((uint64_t) st.st_size < حجم(attr))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, حجم(attr));
                إذا ((uint64_t) st.st_size > حجم(attr) + 4 * U64_MB)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                إذا (!ret_attribute && !ret_مقدار) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        اكسر;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_اندازه = (size_t) st.st_size, payload_اندازه, read_اندازه, alloc_اندازه, read_حد;
                إذا (!SUB_امن(&payload_اندازه, file_اندازه, حجم(attr)) ||
                    !جمع_کن_امن(&read_اندازه, payload_اندازه, 1) ||
                    !جمع_کن_امن(&alloc_اندازه, read_اندازه, 3) ||
                    !جمع_کن_امن(&read_حد, file_اندازه, 1))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                حرر(buf);
                buf = حافظه_بده(alloc_اندازه);
                إذا (!buf)
                        أرجع -ENOMEM;

                بنية iovec iov[] = {
                        { &attr, حجم(attr)                           },
                        { buf,   read_اندازه                              },
                };

                n = readv(fd, iov, 2);
                إذا (n < 0) {
                        إذا (errno != EINTR)
                                أرجع log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } وإلا إذا ((size_t) n == read_حد)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                وإلا {
                        أكد((size_t) n < read_حد);
                        اكسر;
                }

                إذا (try >= EFI_N_RETRIES_مجموع)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                إذا (try >= EFI_N_RETRIES_NO_DELAY)
                        (فراغ) usleep_امن(EFI_RETRY_DELAY);

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
        إذا (n == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        إذا ((size_t) n < حجم(attr))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, حجم(attr));
        size_t مقدار_اندازه = n - حجم(attr);

        إذا (ret_attribute)
                *ret_attribute = attr;

        إذا (ret_مقدار) {
                أكد(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[مقدار_اندازه] = 0;
                buf[مقدار_اندازه + 1] = 0;
                buf[مقدار_اندازه + 2] = 0;
                *ret_مقدار = TAKE_PTR(buf);
        }

        إذا (DEBUG_LOGGING) {
                usec_t انتها = now(CLOCK_MONOTONIC);
                إذا (انتها > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(انتها - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        إذا (ret_اندازه)
                *ret_اندازه = مقدار_اندازه;

        أرجع 0;
}

صحيح efi_get_variable_string(ثابت حرف *variable, حرف **ret) {
        _cleanup_free_ فراغ *s = عدم, *x = عدم;
        size_t ss = 0;
        صحيح r;

        أكد(variable);

        r = efi_get_variable(variable, عدم, &s, &ss);
        إذا (r < 0)
                أرجع r;

        x = utf16_مقصد_utf8(s, ss);
        إذا (!x)
                أرجع -ENOMEM;

        إذا (ret)
                *ret = TAKE_PTR(x);

        أرجع 0;
}

صحيح efi_get_variable_مسیر(ثابت حرف *variable, حرف **ret) {
        صحيح r;

        أكد(variable);

        r = efi_get_variable_string(variable, ret);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                efi_tilt_backslashes(*ret);

        أرجع r;
}

ساكن صحيح efi_verify_variable(ثابت حرف *variable, uint32_t attr, ثابت فراغ *مقدار, size_t اندازه) {
        _cleanup_free_ فراغ *buf = عدم;
        size_t n;
        uint32_t a;
        صحيح r;

        أكد(variable);
        أكد(مقدار || اندازه == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        إذا (r < 0)
                أرجع r;

        أرجع a == attr && memcmp_nn(buf, n, مقدار, اندازه) == 0;
}

صحيح efi_مجموعه_variable(ثابت حرف *variable, ثابت فراغ *مقدار, size_t اندازه) {
        ساكن ثابت uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ بنية var {
                uint32_t attr;
                حرف buf[];
        } _packed_ *buf = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        bool saved_flags_valid = false;
        بیعلامت saved_flags;
        صحيح r;

        أكد(variable);
        أكد(مقدار || اندازه == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        إذا (اندازه > 0 && efi_verify_variable(variable, attr, مقدار, اندازه) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                أرجع 0;
        }

        ثابت حرف *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_پر(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ عدم,
                        /* flags= */ 0);
        إذا (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        إذا (اندازه == 0) {
                إذا (unlink(p) < 0) {
                        r = -errno;
                        اذهب پایان;
                }

                أرجع 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        إذا (fd < 0) {
                r = -errno;
                اذهب پایان;
        }

        buf = حافظه_بده(حجم(uint32_t) + اندازه);
        إذا (!buf) {
                r = -ENOMEM;
                اذهب پایان;
        }

        buf->attr = attr;
        کپی_حافظه(buf->buf, مقدار, اندازه);

        r = loop_write(fd, buf, حجم(uint32_t) + اندازه);
        إذا (r < 0)
                اذهب پایان;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        إذا (futimens(fd, /* times= */ عدم) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

پایان:
        إذا (saved_flags_valid) {
                صحيح q;

                /* Restore the original flags field, just in case */
                إذا (fd < 0)
                        q = chattr_مسیر(p, saved_flags, FS_IMMUTABLE_FL);
                وإلا
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                إذا (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        أرجع r;
}

صحيح efi_مجموعه_variable_string(ثابت حرف *variable, ثابت حرف *مقدار) {
        _cleanup_free_ char16_t *u16 = عدم;

        u16 = utf8_مقصد_utf16(مقدار, SIZE_MAX);
        إذا (!u16)
                أرجع -ENOMEM;

        أرجع efi_مجموعه_variable(variable, u16, (char16_strlen(u16) + 1) * حجم(char16_t));
}

ساكن صحيح cache_efi_boot = -1;

bool مجموعه_efi_boot(bool b) {
        أرجع (cache_efi_boot = b);
}

bool is_efi_boot(فراغ) {
        إذا (cache_efi_boot >= 0)
                أرجع cache_efi_boot;

        إذا (detect_container() > 0)
                أرجع (cache_efi_boot = false);

        إذا (access("/sys/firmware/efi/", F_OK) < 0) {
                إذا (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                أرجع (cache_efi_boot = false);
        }

        أرجع (cache_efi_boot = true);
}

ساكن صحيح read_flag(ثابت حرف *variable) {
        _cleanup_free_ فراغ *v = عدم;
        uint8_t b;
        size_t s;
        صحيح r;

        إذا (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                أرجع 0;

        r = efi_get_variable(variable, عدم, &v, &s);
        إذا (r < 0)
                أرجع r;

        إذا (s != 1)
                أرجع -EINVAL;

        b = *(uint8_t *)v;
        أرجع !!b;
}

bool is_efi_secure_boot(فراغ) {
        ساكن صحيح cache = -1;
        صحيح r;

        إذا (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                إذا (r == -ENOENT)
                        cache = false;
                وإلا إذا (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                وإلا
                        cache = r;
        }

        أرجع cache > 0;
}

Secure_Boot_نما efi_get_secure_boot_نما(فراغ) {
        ساكن Secure_Boot_نما cache = _SECURE_BOOT_INVALID;

        إذا (cache != _SECURE_BOOT_INVALID)
                أرجع cache;

        صحيح secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        إذا (secure < 0) {
                إذا (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                أرجع (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        صحيح audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        صحيح deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        صحيح راهاندازی    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        صحيح moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, راهاندازی, moksb);

        أرجع (cache = decode_secure_boot_نما(secure, audit > 0, deployed > 0, راهاندازی > 0, moksb > 0));
}
#پایان_اگر

حرف *efi_tilt_backslashes(حرف *s) {
        أرجع string_replace_char(s, '\\', '/');
}
