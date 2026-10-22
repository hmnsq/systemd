/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/uio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chattr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "efivars.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "virt.h"

#ਜੇ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_N_RETRIES_NO_DELAY 20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_N_RETRIES_ਕੁੱਲ 25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_RETRY_DELAY (50 * USEC_ਪ੍ਰਤੀ_MSEC)

ਪੂਰਨ efi_get_variable(
                ਅਚਲ ਅੱਖਰ *variable,
                uint32_t *ret_attribute,
                ਖਾਲੀ **ret_ਮੁੱਲ,
                size_t *ret_ਮਾਪ) {

        ਪੂਰਨ r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        ਪ੍ਰਤਿਜ੍ਞਾ(variable);

        ਅਚਲ ਅੱਖਰ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        ਜੇ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ ਪੂਰਨ fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
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
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ try = 0;; try++) {
                ਬਣਤਰ stat st;

                ਜੇ (fstat(fd, &st) < 0)
                        ਵਾਪਸ log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                ਜੇ (st.st_size == 0) /* for uncommitted variables, see below */
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                ਜੇ ((uint64_t) st.st_size < ਆਕਾਰ(attr))
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, ਆਕਾਰ(attr));
                ਜੇ ((uint64_t) st.st_size > ਆਕਾਰ(attr) + 4 * U64_MB)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                ਜੇ (!ret_attribute && !ret_ਮੁੱਲ) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        ਰੋਕੋ;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_ਮਾਪ = (size_t) st.st_size, payload_ਮਾਪ, read_ਮਾਪ, alloc_ਮਾਪ, read_ਹੱਦਬੰਦੀ;
                ਜੇ (!SUB_ਸੁਰੱਖਿਅਤ(&payload_ਮਾਪ, file_ਮਾਪ, ਆਕਾਰ(attr)) ||
                    !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&read_ਮਾਪ, payload_ਮਾਪ, 1) ||
                    !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&alloc_ਮਾਪ, read_ਮਾਪ, 3) ||
                    !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&read_ਹੱਦਬੰਦੀ, file_ਮਾਪ, 1))
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                ਮੁਕਤ(buf);
                buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(alloc_ਮਾਪ);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                ਬਣਤਰ iovec iov[] = {
                        { &attr, ਆਕਾਰ(attr)                           },
                        { buf,   read_ਮਾਪ                              },
                };

                n = readv(fd, iov, 2);
                ਜੇ (n < 0) {
                        ਜੇ (errno != EINTR)
                                ਵਾਪਸ log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } ਨਹੀਂਤਾਂ ਜੇ ((size_t) n == read_ਹੱਦਬੰਦੀ)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                ਨਹੀਂਤਾਂ {
                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) n < read_ਹੱਦਬੰਦੀ);
                        ਰੋਕੋ;
                }

                ਜੇ (try >= EFI_N_RETRIES_ਕੁੱਲ)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                ਜੇ (try >= EFI_N_RETRIES_NO_DELAY)
                        (ਖਾਲੀ) usleep_ਸੁਰੱਖਿਅਤ(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (ਖਾਲੀ) lseek(fd, 0, SEEK_SET);
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
        ਜੇ (n == 0)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        ਜੇ ((size_t) n < ਆਕਾਰ(attr))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, ਆਕਾਰ(attr));
        size_t ਮੁੱਲ_ਮਾਪ = n - ਆਕਾਰ(attr);

        ਜੇ (ret_attribute)
                *ret_attribute = attr;

        ਜੇ (ret_ਮੁੱਲ) {
                ਪ੍ਰਤਿਜ੍ਞਾ(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[ਮੁੱਲ_ਮਾਪ] = 0;
                buf[ਮੁੱਲ_ਮਾਪ + 1] = 0;
                buf[ਮੁੱਲ_ਮਾਪ + 2] = 0;
                *ret_ਮੁੱਲ = TAKE_PTR(buf);
        }

        ਜੇ (DEBUG_LOGGING) {
                usec_t ਸਿਰਾ = now(CLOCK_MONOTONIC);
                ਜੇ (ਸਿਰਾ > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(ਸਿਰਾ - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = ਮੁੱਲ_ਮਾਪ;

        ਵਾਪਸ 0;
}

ਪੂਰਨ efi_get_variable_string(ਅਚਲ ਅੱਖਰ *variable, ਅੱਖਰ **ret) {
        _cleanup_free_ ਖਾਲੀ *s = ਨਿਰਾ, *x = ਨਿਰਾ;
        size_t ss = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(variable);

        r = efi_get_variable(variable, ਨਿਰਾ, &s, &ss);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        x = utf16_ਮੰਜ਼ਿਲ_utf8(s, ss);
        ਜੇ (!x)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (ret)
                *ret = TAKE_PTR(x);

        ਵਾਪਸ 0;
}

ਪੂਰਨ efi_get_variable_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *variable, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(variable);

        r = efi_get_variable_string(variable, ret);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                efi_tilt_backslashes(*ret);

        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ efi_verify_variable(ਅਚਲ ਅੱਖਰ *variable, uint32_t attr, ਅਚਲ ਖਾਲੀ *ਮੁੱਲ, size_t ਮਾਪ) {
        _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;
        size_t n;
        uint32_t a;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(variable);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ || ਮਾਪ == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ a == attr && memcmp_nn(buf, n, ਮੁੱਲ, ਮਾਪ) == 0;
}

ਪੂਰਨ efi_ਸੈੱਟ_variable(ਅਚਲ ਅੱਖਰ *variable, ਅਚਲ ਖਾਲੀ *ਮੁੱਲ, size_t ਮਾਪ) {
        ਸਥਿਰ ਅਚਲ uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ਬਣਤਰ var {
                uint32_t attr;
                ਅੱਖਰ buf[];
        } _packed_ *buf = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        bool saved_flags_valid = false;
        ਬਿਨਾਨਿਸ਼ਾਨੀ saved_flags;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(variable);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ || ਮਾਪ == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        ਜੇ (ਮਾਪ > 0 && efi_verify_variable(variable, attr, ਮੁੱਲ, ਮਾਪ) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ਵਾਪਸ 0;
        }

        ਅਚਲ ਅੱਖਰ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_ਭਰਿਆ(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ਨਿਰਾ,
                        /* flags= */ 0);
        ਜੇ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        ਜੇ (ਮਾਪ == 0) {
                ਜੇ (unlink(p) < 0) {
                        r = -errno;
                        ਜਾਓ ਅੰਤ;
                }

                ਵਾਪਸ 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        ਜੇ (fd < 0) {
                r = -errno;
                ਜਾਓ ਅੰਤ;
        }

        buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(uint32_t) + ਮਾਪ);
        ਜੇ (!buf) {
                r = -ENOMEM;
                ਜਾਓ ਅੰਤ;
        }

        buf->attr = attr;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf->buf, ਮੁੱਲ, ਮਾਪ);

        r = loop_write(fd, buf, ਆਕਾਰ(uint32_t) + ਮਾਪ);
        ਜੇ (r < 0)
                ਜਾਓ ਅੰਤ;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        ਜੇ (futimens(fd, /* times= */ ਨਿਰਾ) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

ਅੰਤ:
        ਜੇ (saved_flags_valid) {
                ਪੂਰਨ q;

                /* Restore the original flags field, just in case */
                ਜੇ (fd < 0)
                        q = chattr_ਰਸਤਾ(p, saved_flags, FS_IMMUTABLE_FL);
                ਨਹੀਂਤਾਂ
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                ਜੇ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ efi_ਸੈੱਟ_variable_string(ਅਚਲ ਅੱਖਰ *variable, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        _cleanup_free_ char16_t *u16 = ਨਿਰਾ;

        u16 = utf8_ਮੰਜ਼ਿਲ_utf16(ਮੁੱਲ, SIZE_MAX);
        ਜੇ (!u16)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ efi_ਸੈੱਟ_variable(variable, u16, (char16_strlen(u16) + 1) * ਆਕਾਰ(char16_t));
}

ਸਥਿਰ ਪੂਰਨ cache_efi_boot = -1;

bool ਸੈੱਟ_efi_boot(bool b) {
        ਵਾਪਸ (cache_efi_boot = b);
}

bool is_efi_boot(ਖਾਲੀ) {
        ਜੇ (cache_efi_boot >= 0)
                ਵਾਪਸ cache_efi_boot;

        ਜੇ (detect_container() > 0)
                ਵਾਪਸ (cache_efi_boot = false);

        ਜੇ (access("/sys/firmware/efi/", F_OK) < 0) {
                ਜੇ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ਵਾਪਸ (cache_efi_boot = false);
        }

        ਵਾਪਸ (cache_efi_boot = true);
}

ਸਥਿਰ ਪੂਰਨ read_flag(ਅਚਲ ਅੱਖਰ *variable) {
        _cleanup_free_ ਖਾਲੀ *v = ਨਿਰਾ;
        uint8_t b;
        size_t s;
        ਪੂਰਨ r;

        ਜੇ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ਵਾਪਸ 0;

        r = efi_get_variable(variable, ਨਿਰਾ, &v, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (s != 1)
                ਵਾਪਸ -EINVAL;

        b = *(uint8_t *)v;
        ਵਾਪਸ !!b;
}

bool is_efi_secure_boot(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ cache = -1;
        ਪੂਰਨ r;

        ਜੇ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                ਜੇ (r == -ENOENT)
                        cache = false;
                ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                ਨਹੀਂਤਾਂ
                        cache = r;
        }

        ਵਾਪਸ cache > 0;
}

Secure_Boot_ਬਹੁਲਕ efi_get_secure_boot_ਬਹੁਲਕ(ਖਾਲੀ) {
        ਸਥਿਰ Secure_Boot_ਬਹੁਲਕ cache = _SECURE_BOOT_INVALID;

        ਜੇ (cache != _SECURE_BOOT_INVALID)
                ਵਾਪਸ cache;

        ਪੂਰਨ secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        ਜੇ (secure < 0) {
                ਜੇ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ਵਾਪਸ (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        ਪੂਰਨ audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        ਪੂਰਨ deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        ਪੂਰਨ ਤਿਆਰੀ    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        ਪੂਰਨ moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, ਤਿਆਰੀ, moksb);

        ਵਾਪਸ (cache = decode_secure_boot_ਬਹੁਲਕ(secure, audit > 0, deployed > 0, ਤਿਆਰੀ > 0, moksb > 0));
}
#ਜੇ_ਅੰਤ

ਅੱਖਰ *efi_tilt_backslashes(ਅੱਖਰ *s) {
        ਵਾਪਸ string_replace_char(s, '\\', '/');
}
