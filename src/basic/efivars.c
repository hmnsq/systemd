/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <sys/uio.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chattr-util.h"
#ಅನ್ತರ್ಭಾವ "efivars.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"
#ಅನ್ತರ್ಭಾವ "virt.h"

#ಆದರೆ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_N_RETRIES_NO_DELAY 20
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_N_RETRIES_ಒಟ್ಟು 25
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_RETRY_DELAY (50 * USEC_ಪ್ರತಿ_MSEC)

ಪೂರ್ಣಾಂಕ efi_get_variable(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable,
                uint32_t *ret_attribute,
                ಶೂನ್ಯ **ret_ಮೌಲ್ಯ,
                size_t *ret_ಗಾತ್ರ) {

        ಪೂರ್ಣಾಂಕ r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        ಪ್ರತಿಜ್ಞಾ(variable);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        ಆದರೆ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
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
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ try = 0;; try++) {
                ರಚನೆ stat st;

                ಆದರೆ (fstat(fd, &st) < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                ಆದರೆ (st.st_size == 0) /* for uncommitted variables, see below */
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                ಆದರೆ ((uint64_t) st.st_size < ಗಾತ್ರ(attr))
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, ಗಾತ್ರ(attr));
                ಆದರೆ ((uint64_t) st.st_size > ಗಾತ್ರ(attr) + 4 * U64_MB)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                ಆದರೆ (!ret_attribute && !ret_ಮೌಲ್ಯ) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        ನಿಲ್ಲಿಸು;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_ಗಾತ್ರ = (size_t) st.st_size, payload_ಗಾತ್ರ, read_ಗಾತ್ರ, alloc_ಗಾತ್ರ, read_ಎಲ್ಲೆ;
                ಆದರೆ (!SUB_ಸುರಕ್ಷಿತ(&payload_ಗಾತ್ರ, file_ಗಾತ್ರ, ಗಾತ್ರ(attr)) ||
                    !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&read_ಗಾತ್ರ, payload_ಗಾತ್ರ, 1) ||
                    !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&alloc_ಗಾತ್ರ, read_ಗಾತ್ರ, 3) ||
                    !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&read_ಎಲ್ಲೆ, file_ಗಾತ್ರ, 1))
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                ಬಿಡುಗಡೆ(buf);
                buf = ಸ್ಮರಣೆ_ಕೊಡು(alloc_ಗಾತ್ರ);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ರಚನೆ iovec iov[] = {
                        { &attr, ಗಾತ್ರ(attr)                           },
                        { buf,   read_ಗಾತ್ರ                              },
                };

                n = readv(fd, iov, 2);
                ಆದರೆ (n < 0) {
                        ಆದರೆ (errno != EINTR)
                                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((size_t) n == read_ಎಲ್ಲೆ)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                ಇಲ್ಲದಿದ್ದರೆ {
                        ಪ್ರತಿಜ್ಞಾ((size_t) n < read_ಎಲ್ಲೆ);
                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (try >= EFI_N_RETRIES_ಒಟ್ಟು)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                ಆದರೆ (try >= EFI_N_RETRIES_NO_DELAY)
                        (ಶೂನ್ಯ) usleep_ಸುರಕ್ಷಿತ(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (ಶೂನ್ಯ) lseek(fd, 0, SEEK_SET);
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
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        ಆದರೆ ((size_t) n < ಗಾತ್ರ(attr))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, ಗಾತ್ರ(attr));
        size_t ಮೌಲ್ಯ_ಗಾತ್ರ = n - ಗಾತ್ರ(attr);

        ಆದರೆ (ret_attribute)
                *ret_attribute = attr;

        ಆದರೆ (ret_ಮೌಲ್ಯ) {
                ಪ್ರತಿಜ್ಞಾ(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[ಮೌಲ್ಯ_ಗಾತ್ರ] = 0;
                buf[ಮೌಲ್ಯ_ಗಾತ್ರ + 1] = 0;
                buf[ಮೌಲ್ಯ_ಗಾತ್ರ + 2] = 0;
                *ret_ಮೌಲ್ಯ = TAKE_PTR(buf);
        }

        ಆದರೆ (DEBUG_LOGGING) {
                usec_t ಕೊನೆ = now(CLOCK_MONOTONIC);
                ಆದರೆ (ಕೊನೆ > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(ಕೊನೆ - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = ಮೌಲ್ಯ_ಗಾತ್ರ;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ efi_get_variable_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಶೂನ್ಯ *s = ಖಾಲಿ, *x = ಖಾಲಿ;
        size_t ss = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);

        r = efi_get_variable(variable, ಖಾಲಿ, &s, &ss);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        x = utf16_ಗಮ್ಯ_utf8(s, ss);
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(x);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ efi_get_variable_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);

        r = efi_get_variable_string(variable, ret);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                efi_tilt_backslashes(*ret);

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ efi_verify_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, uint32_t attr, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t ಗಾತ್ರ_2) {
        _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;
        size_t n;
        uint32_t a;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || ಗಾತ್ರ_2 == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ a == attr && memcmp_nn(buf, n, ಮೌಲ್ಯ, ಗಾತ್ರ_2) == 0;
}

ಪೂರ್ಣಾಂಕ efi_ಗಣ_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t ಗಾತ್ರ_2) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ರಚನೆ var {
                uint32_t attr;
                ಅಕ್ಷರ buf[];
        } _packed_ *buf = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        bool saved_flags_valid = false;
        ಚಿಹ್ನೆರಹಿತ saved_flags;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || ಗಾತ್ರ_2 == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        ಆದರೆ (ಗಾತ್ರ_2 > 0 && efi_verify_variable(variable, attr, ಮೌಲ್ಯ, ಗಾತ್ರ_2) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_ತುಂಬಿದ(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ಖಾಲಿ,
                        /* flags= */ 0);
        ಆದರೆ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        ಆದರೆ (ಗಾತ್ರ_2 == 0) {
                ಆದರೆ (unlink(p) < 0) {
                        r = -errno;
                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

                ಹಿಂತಿರುಗಿ 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        ಆದರೆ (fd < 0) {
                r = -errno;
                ಹೋಗು ಮುಕ್ತಾಯ;
        }

        buf = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(uint32_t) + ಗಾತ್ರ_2);
        ಆದರೆ (!buf) {
                r = -ENOMEM;
                ಹೋಗು ಮುಕ್ತಾಯ;
        }

        buf->attr = attr;
        ಸ್ಮರಣೆ_ನಕಲು(buf->buf, ಮೌಲ್ಯ, ಗಾತ್ರ_2);

        r = loop_write(fd, buf, ಗಾತ್ರ(uint32_t) + ಗಾತ್ರ_2);
        ಆದರೆ (r < 0)
                ಹೋಗು ಮುಕ್ತಾಯ;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        ಆದರೆ (futimens(fd, /* times= */ ಖಾಲಿ) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

ಮುಕ್ತಾಯ:
        ಆದರೆ (saved_flags_valid) {
                ಪೂರ್ಣಾಂಕ q;

                /* Restore the original flags field, just in case */
                ಆದರೆ (fd < 0)
                        q = chattr_ಮಾರ್ಗ(p, saved_flags, FS_IMMUTABLE_FL);
                ಇಲ್ಲದಿದ್ದರೆ
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                ಆದರೆ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ efi_ಗಣ_variable_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        _cleanup_free_ char16_t *u16 = ಖಾಲಿ;

        u16 = utf8_ಗಮ್ಯ_utf16(ಮೌಲ್ಯ, SIZE_MAX);
        ಆದರೆ (!u16)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ efi_ಗಣ_variable(variable, u16, (char16_strlen(u16) + 1) * ಗಾತ್ರ(char16_t));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cache_efi_boot = -1;

bool ಗಣ_efi_boot(bool b) {
        ಹಿಂತಿರುಗಿ (cache_efi_boot = b);
}

bool is_efi_boot(ಶೂನ್ಯ) {
        ಆದರೆ (cache_efi_boot >= 0)
                ಹಿಂತಿರುಗಿ cache_efi_boot;

        ಆದರೆ (detect_container() > 0)
                ಹಿಂತಿರುಗಿ (cache_efi_boot = false);

        ಆದರೆ (access("/sys/firmware/efi/", F_OK) < 0) {
                ಆದರೆ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ಹಿಂತಿರುಗಿ (cache_efi_boot = false);
        }

        ಹಿಂತಿರುಗಿ (cache_efi_boot = true);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_flag(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable) {
        _cleanup_free_ ಶೂನ್ಯ *v = ಖಾಲಿ;
        uint8_t b;
        size_t s;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ಹಿಂತಿರುಗಿ 0;

        r = efi_get_variable(variable, ಖಾಲಿ, &v, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (s != 1)
                ಹಿಂತಿರುಗಿ -EINVAL;

        b = *(uint8_t *)v;
        ಹಿಂತಿರುಗಿ !!b;
}

bool is_efi_secure_boot(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cache = -1;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                ಆದರೆ (r == -ENOENT)
                        cache = false;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                ಇಲ್ಲದಿದ್ದರೆ
                        cache = r;
        }

        ಹಿಂತಿರುಗಿ cache > 0;
}

Secure_Boot_ಬಹುಲಕ efi_get_secure_boot_ಬಹುಲಕ(ಶೂನ್ಯ) {
        ಸ್ಥಿರ Secure_Boot_ಬಹುಲಕ cache = _SECURE_BOOT_INVALID;

        ಆದರೆ (cache != _SECURE_BOOT_INVALID)
                ಹಿಂತಿರುಗಿ cache;

        ಪೂರ್ಣಾಂಕ secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        ಆದರೆ (secure < 0) {
                ಆದರೆ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ಹಿಂತಿರುಗಿ (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        ಪೂರ್ಣಾಂಕ audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        ಪೂರ್ಣಾಂಕ deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        ಪೂರ್ಣಾಂಕ ಸಿದ್ಧತೆ    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        ಪೂರ್ಣಾಂಕ moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, ಸಿದ್ಧತೆ, moksb);

        ಹಿಂತಿರುಗಿ (cache = decode_secure_boot_ಬಹುಲಕ(secure, audit > 0, deployed > 0, ಸಿದ್ಧತೆ > 0, moksb > 0));
}
#ಆದರೆ_ಅಂತ್ಯ

ಅಕ್ಷರ *efi_tilt_backslashes(ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ string_replace_char(s, '\\', '/');
}
