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

#ಯದಿ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

ಪೂರ್ಣಾಙ್ಕ efi_get_variable(
                ಧ್ರುವ ವರ್ಣ *variable,
                uint32_t *ret_attribute,
                ಶೂನ್ಯ **ret_ಮೌಲ್ಯ,
                size_t *ret_size) {

        ಪೂರ್ಣಾಙ್ಕ r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        ಪ್ರತಿಜ್ಞಾ(variable);

        ಧ್ರುವ ವರ್ಣ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        ಯದಿ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
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
        ಪ್ರತಿ (unsigned try = 0;; try++) {
                ಸಂರಚನಾ stat st;

                ಯದಿ (fstat(fd, &st) < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                ಯದಿ (st.st_size == 0) /* for uncommitted variables, see below */
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                ಯದಿ ((uint64_t) st.st_size < ಪರಿಮಾಣ(attr))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, ಪರಿಮಾಣ(attr));
                ಯದಿ ((uint64_t) st.st_size > ಪರಿಮಾಣ(attr) + 4 * U64_MB)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                ಯದಿ (!ret_attribute && !ret_ಮೌಲ್ಯ) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        ಭಙ್ಗ;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                ಯದಿ (!SUB_SAFE(&payload_size, file_size, ಪರಿಮಾಣ(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                ಮುಕ್ತ(buf);
                buf = malloc(alloc_size);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಸಂರಚನಾ iovec iov[] = {
                        { &attr, ಪರಿಮಾಣ(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                ಯದಿ (n < 0) {
                        ಯದಿ (errno != EINTR)
                                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } ಅನ್ಯಥಾ ಯದಿ ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                ಅನ್ಯಥಾ {
                        ಪ್ರತಿಜ್ಞಾ((size_t) n < read_limit);
                        ಭಙ್ಗ;
                }

                ಯದಿ (try >= EFI_N_RETRIES_TOTAL)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                ಯದಿ (try >= EFI_N_RETRIES_NO_DELAY)
                        (ಶೂನ್ಯ) usleep_safe(EFI_RETRY_DELAY);

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
        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        ಯದಿ ((size_t) n < ಪರಿಮಾಣ(attr))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, ಪರಿಮಾಣ(attr));
        size_t ಮೌಲ್ಯ_size = n - ಪರಿಮಾಣ(attr);

        ಯದಿ (ret_attribute)
                *ret_attribute = attr;

        ಯದಿ (ret_ಮೌಲ್ಯ) {
                ಪ್ರತಿಜ್ಞಾ(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[ಮೌಲ್ಯ_size] = 0;
                buf[ಮೌಲ್ಯ_size + 1] = 0;
                buf[ಮೌಲ್ಯ_size + 2] = 0;
                *ret_ಮೌಲ್ಯ = TAKE_PTR(buf);
        }

        ಯದಿ (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                ಯದಿ (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        ಯದಿ (ret_size)
                *ret_size = ಮೌಲ್ಯ_size;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ efi_get_variable_string(ಧ್ರುವ ವರ್ಣ *variable, ವರ್ಣ **ret) {
        _cleanup_free_ ಶೂನ್ಯ *s = ರಿಕ್ತ, *x = ರಿಕ್ತ;
        size_t ss = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);

        r = efi_get_variable(variable, ರಿಕ್ತ, &s, &ss);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        x = utf16_to_utf8(s, ss);
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (ret)
                *ret = TAKE_PTR(x);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ efi_get_variable_path(ಧ್ರುವ ವರ್ಣ *variable, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);

        r = efi_get_variable_string(variable, ret);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                efi_tilt_backslashes(*ret);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ efi_verify_variable(ಧ್ರುವ ವರ್ಣ *variable, uint32_t attr, ಧ್ರುವ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t size) {
        _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;
        size_t n;
        uint32_t a;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ a == attr && memcmp_nn(buf, n, ಮೌಲ್ಯ, size) == 0;
}

ಪೂರ್ಣಾಙ್ಕ efi_set_variable(ಧ್ರುವ ವರ್ಣ *variable, ಧ್ರುವ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t size) {
        ಸ್ಥಿರ ಧ್ರುವ uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ಸಂರಚನಾ var {
                uint32_t attr;
                ವರ್ಣ buf[];
        } _packed_ *buf = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(variable);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        ಯದಿ (size > 0 && efi_verify_variable(variable, attr, ಮೌಲ್ಯ, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಧ್ರುವ ವರ್ಣ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ರಿಕ್ತ,
                        /* flags= */ 0);
        ಯದಿ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        ಯದಿ (size == 0) {
                ಯದಿ (unlink(p) < 0) {
                        r = -errno;
                        ಗಚ್ಛ finish;
                }

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        ಯದಿ (fd < 0) {
                r = -errno;
                ಗಚ್ಛ finish;
        }

        buf = malloc(ಪರಿಮಾಣ(uint32_t) + size);
        ಯದಿ (!buf) {
                r = -ENOMEM;
                ಗಚ್ಛ finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, ಮೌಲ್ಯ, size);

        r = loop_write(fd, buf, ಪರಿಮಾಣ(uint32_t) + size);
        ಯದಿ (r < 0)
                ಗಚ್ಛ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        ಯದಿ (futimens(fd, /* times= */ ರಿಕ್ತ) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        ಯದಿ (saved_flags_valid) {
                ಪೂರ್ಣಾಙ್ಕ q;

                /* Restore the original flags field, just in case */
                ಯದಿ (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                ಅನ್ಯಥಾ
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                ಯದಿ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ efi_set_variable_string(ಧ್ರುವ ವರ್ಣ *variable, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        _cleanup_free_ char16_t *u16 = ರಿಕ್ತ;

        u16 = utf8_to_utf16(ಮೌಲ್ಯ, SIZE_MAX);
        ಯದಿ (!u16)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * ಪರಿಮಾಣ(char16_t));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        ಪ್ರತ್ಯಾಗಮ (cache_efi_boot = b);
}

bool is_efi_boot(ಶೂನ್ಯ) {
        ಯದಿ (cache_efi_boot >= 0)
                ಪ್ರತ್ಯಾಗಮ cache_efi_boot;

        ಯದಿ (detect_container() > 0)
                ಪ್ರತ್ಯಾಗಮ (cache_efi_boot = false);

        ಯದಿ (access("/sys/firmware/efi/", F_OK) < 0) {
                ಯದಿ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ಪ್ರತ್ಯಾಗಮ (cache_efi_boot = false);
        }

        ಪ್ರತ್ಯಾಗಮ (cache_efi_boot = true);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_flag(ಧ್ರುವ ವರ್ಣ *variable) {
        _cleanup_free_ ಶೂನ್ಯ *v = ರಿಕ್ತ;
        uint8_t b;
        size_t s;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ಪ್ರತ್ಯಾಗಮ 0;

        r = efi_get_variable(variable, ರಿಕ್ತ, &v, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (s != 1)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        b = *(uint8_t *)v;
        ಪ್ರತ್ಯಾಗಮ !!b;
}

bool is_efi_secure_boot(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cache = -1;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                ಯದಿ (r == -ENOENT)
                        cache = false;
                ಅನ್ಯಥಾ ಯದಿ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                ಅನ್ಯಥಾ
                        cache = r;
        }

        ಪ್ರತ್ಯಾಗಮ cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(ಶೂನ್ಯ) {
        ಸ್ಥಿರ SecureBootMode cache = _SECURE_BOOT_INVALID;

        ಯದಿ (cache != _SECURE_BOOT_INVALID)
                ಪ್ರತ್ಯಾಗಮ cache;

        ಪೂರ್ಣಾಙ್ಕ secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        ಯದಿ (secure < 0) {
                ಯದಿ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ಪ್ರತ್ಯಾಗಮ (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        ಪೂರ್ಣಾಙ್ಕ audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        ಪೂರ್ಣಾಙ್ಕ deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        ಪೂರ್ಣಾಙ್ಕ setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        ಪೂರ್ಣಾಙ್ಕ moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        ಪ್ರತ್ಯಾಗಮ (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

ವರ್ಣ *efi_tilt_backslashes(ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ string_replace_char(s, '\\', '/');
}
