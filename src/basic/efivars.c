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
#تعریف_کرو EFI_N_RETRIES_NO_DELAY 20
#تعریف_کرو EFI_N_RETRIES_کل 25
#تعریف_کرو EFI_RETRY_DELAY (50 * USEC_فی_MSEC)

صحیح efi_get_variable(
                ثابت حرف *variable,
                uint32_t *ret_attribute,
                خالی **ret_قدر,
                size_t *ret_سائز) {

        صحیح r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        أكد(variable);

        ثابت حرف *p = strjoina("/sys/firmware/efi/efivars/", variable);

        اگر (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ صحیح fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                واپس log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ حرف *buf = تہی;
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
        برائے (غیرعلامتی try = 0;; try++) {
                ساخت stat st;

                اگر (fstat(fd, &st) < 0)
                        واپس log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                اگر (r < 0)
                        واپس log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                اگر (st.st_size == 0) /* for uncommitted variables, see below */
                        واپس log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                اگر ((uint64_t) st.st_size < حجم(attr))
                        واپس log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, حجم(attr));
                اگر ((uint64_t) st.st_size > حجم(attr) + 4 * U64_MB)
                        واپس log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                اگر (!ret_attribute && !ret_قدر) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        توڑو;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_سائز = (size_t) st.st_size, payload_سائز, read_سائز, alloc_سائز, read_حد_بندی;
                اگر (!SUB_محفوظ(&payload_سائز, file_سائز, حجم(attr)) ||
                    !جوڑو_محفوظ(&read_سائز, payload_سائز, 1) ||
                    !جوڑو_محفوظ(&alloc_سائز, read_سائز, 3) ||
                    !جوڑو_محفوظ(&read_حد_بندی, file_سائز, 1))
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                آزاد(buf);
                buf = حافظہ_دو(alloc_سائز);
                اگر (!buf)
                        واپس -ENOMEM;

                ساخت iovec iov[] = {
                        { &attr, حجم(attr)                           },
                        { buf,   read_سائز                              },
                };

                n = readv(fd, iov, 2);
                اگر (n < 0) {
                        اگر (errno != EINTR)
                                واپس log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } ورنہ اگر ((size_t) n == read_حد_بندی)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                ورنہ {
                        أكد((size_t) n < read_حد_بندی);
                        توڑو;
                }

                اگر (try >= EFI_N_RETRIES_کل)
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                اگر (try >= EFI_N_RETRIES_NO_DELAY)
                        (خالی) usleep_محفوظ(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (خالی) lseek(fd, 0, SEEK_SET);
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
                واپس log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        اگر ((size_t) n < حجم(attr))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, حجم(attr));
        size_t قدر_سائز = n - حجم(attr);

        اگر (ret_attribute)
                *ret_attribute = attr;

        اگر (ret_قدر) {
                أكد(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[قدر_سائز] = 0;
                buf[قدر_سائز + 1] = 0;
                buf[قدر_سائز + 2] = 0;
                *ret_قدر = TAKE_PTR(buf);
        }

        اگر (DEBUG_LOGGING) {
                usec_t انتہا = now(CLOCK_MONOTONIC);
                اگر (انتہا > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(انتہا - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        اگر (ret_سائز)
                *ret_سائز = قدر_سائز;

        واپس 0;
}

صحیح efi_get_variable_string(ثابت حرف *variable, حرف **ret) {
        _cleanup_free_ خالی *s = تہی, *x = تہی;
        size_t ss = 0;
        صحیح r;

        أكد(variable);

        r = efi_get_variable(variable, تہی, &s, &ss);
        اگر (r < 0)
                واپس r;

        x = utf16_مقصد_utf8(s, ss);
        اگر (!x)
                واپس -ENOMEM;

        اگر (ret)
                *ret = TAKE_PTR(x);

        واپس 0;
}

صحیح efi_get_variable_راستہ(ثابت حرف *variable, حرف **ret) {
        صحیح r;

        أكد(variable);

        r = efi_get_variable_string(variable, ret);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                efi_tilt_backslashes(*ret);

        واپس r;
}

ساکن صحیح efi_verify_variable(ثابت حرف *variable, uint32_t attr, ثابت خالی *قدر, size_t سائز) {
        _cleanup_free_ خالی *buf = تہی;
        size_t n;
        uint32_t a;
        صحیح r;

        أكد(variable);
        أكد(قدر || سائز == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        اگر (r < 0)
                واپس r;

        واپس a == attr && memcmp_nn(buf, n, قدر, سائز) == 0;
}

صحیح efi_سیٹ_variable(ثابت حرف *variable, ثابت خالی *قدر, size_t سائز) {
        ساکن ثابت uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ساخت var {
                uint32_t attr;
                حرف buf[];
        } _packed_ *buf = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        bool saved_flags_valid = false;
        غیرعلامتی saved_flags;
        صحیح r;

        أكد(variable);
        أكد(قدر || سائز == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        اگر (سائز > 0 && efi_verify_variable(variable, attr, قدر, سائز) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                واپس 0;
        }

        ثابت حرف *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_بھرا(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ تہی,
                        /* flags= */ 0);
        اگر (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        اگر (سائز == 0) {
                اگر (unlink(p) < 0) {
                        r = -errno;
                        جاؤ اختتام;
                }

                واپس 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        اگر (fd < 0) {
                r = -errno;
                جاؤ اختتام;
        }

        buf = حافظہ_دو(حجم(uint32_t) + سائز);
        اگر (!buf) {
                r = -ENOMEM;
                جاؤ اختتام;
        }

        buf->attr = attr;
        حافظہ_نقل(buf->buf, قدر, سائز);

        r = loop_write(fd, buf, حجم(uint32_t) + سائز);
        اگر (r < 0)
                جاؤ اختتام;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        اگر (futimens(fd, /* times= */ تہی) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

اختتام:
        اگر (saved_flags_valid) {
                صحیح q;

                /* Restore the original flags field, just in case */
                اگر (fd < 0)
                        q = chattr_راستہ(p, saved_flags, FS_IMMUTABLE_FL);
                ورنہ
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                اگر (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        واپس r;
}

صحیح efi_سیٹ_variable_string(ثابت حرف *variable, ثابت حرف *قدر) {
        _cleanup_free_ char16_t *u16 = تہی;

        u16 = utf8_مقصد_utf16(قدر, SIZE_MAX);
        اگر (!u16)
                واپس -ENOMEM;

        واپس efi_سیٹ_variable(variable, u16, (char16_strlen(u16) + 1) * حجم(char16_t));
}

ساکن صحیح cache_efi_boot = -1;

bool سیٹ_efi_boot(bool b) {
        واپس (cache_efi_boot = b);
}

bool is_efi_boot(خالی) {
        اگر (cache_efi_boot >= 0)
                واپس cache_efi_boot;

        اگر (detect_container() > 0)
                واپس (cache_efi_boot = false);

        اگر (access("/sys/firmware/efi/", F_OK) < 0) {
                اگر (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                واپس (cache_efi_boot = false);
        }

        واپس (cache_efi_boot = true);
}

ساکن صحیح read_flag(ثابت حرف *variable) {
        _cleanup_free_ خالی *v = تہی;
        uint8_t b;
        size_t s;
        صحیح r;

        اگر (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                واپس 0;

        r = efi_get_variable(variable, تہی, &v, &s);
        اگر (r < 0)
                واپس r;

        اگر (s != 1)
                واپس -EINVAL;

        b = *(uint8_t *)v;
        واپس !!b;
}

bool is_efi_secure_boot(خالی) {
        ساکن صحیح cache = -1;
        صحیح r;

        اگر (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                اگر (r == -ENOENT)
                        cache = false;
                ورنہ اگر (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                ورنہ
                        cache = r;
        }

        واپس cache > 0;
}

Secure_Boot_منوال efi_get_secure_boot_منوال(خالی) {
        ساکن Secure_Boot_منوال cache = _SECURE_BOOT_INVALID;

        اگر (cache != _SECURE_BOOT_INVALID)
                واپس cache;

        صحیح secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        اگر (secure < 0) {
                اگر (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                واپس (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        صحیح audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        صحیح deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        صحیح تیاری    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        صحیح moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, تیاری, moksb);

        واپس (cache = decode_secure_boot_منوال(secure, audit > 0, deployed > 0, تیاری > 0, moksb > 0));
}
#اگر_ختم

حرف *efi_tilt_backslashes(حرف *s) {
        واپس string_replace_char(s, '\\', '/');
}
