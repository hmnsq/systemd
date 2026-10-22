/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <sys/uio.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chattr-util.h"
#अन्तर्भाव "efivars.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "utf8.h"
#अन्तर्भाव "virt.h"

#जइ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

पूर्णाङ्क efi_get_variable(
                ध्रुव वर्ण *variable,
                uint32_t *ret_attribute,
                शून्य **ret_मानम्,
                size_t *ret_size) {

        पूर्णाङ्क r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        प्रतिज्ञा(variable);

        ध्रुव वर्ण *p = strjoina("/sys/firmware/efi/efivars/", variable);

        जइ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ पूर्णाङ्क fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        जइ (fd < 0)
                प्रत्यागम log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ वर्ण *buf = रिक्त;
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
        प्रति (unsigned try = 0;; try++) {
                संरचना stat st;

                जइ (fstat(fd, &st) < 0)
                        प्रत्यागम log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                जइ (r < 0)
                        प्रत्यागम log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                जइ (st.st_size == 0) /* for uncommitted variables, see below */
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                जइ ((uint64_t) st.st_size < परिमाण(attr))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, परिमाण(attr));
                जइ ((uint64_t) st.st_size > परिमाण(attr) + 4 * U64_MB)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                जइ (!ret_attribute && !ret_मानम्) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        भङ्ग;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                जइ (!SUB_SAFE(&payload_size, file_size, परिमाण(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                मुक्त(buf);
                buf = malloc(alloc_size);
                जइ (!buf)
                        प्रत्यागम -ENOMEM;

                संरचना iovec iov[] = {
                        { &attr, परिमाण(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                जइ (n < 0) {
                        जइ (errno != EINTR)
                                प्रत्यागम log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } अण्णहा जइ ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                अण्णहा {
                        प्रतिज्ञा((size_t) n < read_limit);
                        भङ्ग;
                }

                जइ (try >= EFI_N_RETRIES_TOTAL)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                जइ (try >= EFI_N_RETRIES_NO_DELAY)
                        (शून्य) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (शून्य) lseek(fd, 0, SEEK_SET);
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
        जइ (n == 0)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        जइ ((size_t) n < परिमाण(attr))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, परिमाण(attr));
        size_t मानम्_size = n - परिमाण(attr);

        जइ (ret_attribute)
                *ret_attribute = attr;

        जइ (ret_मानम्) {
                प्रतिज्ञा(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[मानम्_size] = 0;
                buf[मानम्_size + 1] = 0;
                buf[मानम्_size + 2] = 0;
                *ret_मानम् = TAKE_PTR(buf);
        }

        जइ (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                जइ (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        जइ (ret_size)
                *ret_size = मानम्_size;

        प्रत्यागम 0;
}

पूर्णाङ्क efi_get_variable_string(ध्रुव वर्ण *variable, वर्ण **ret) {
        _cleanup_free_ शून्य *s = रिक्त, *x = रिक्त;
        size_t ss = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);

        r = efi_get_variable(variable, रिक्त, &s, &ss);
        जइ (r < 0)
                प्रत्यागम r;

        x = utf16_to_utf8(s, ss);
        जइ (!x)
                प्रत्यागम -ENOMEM;

        जइ (ret)
                *ret = TAKE_PTR(x);

        प्रत्यागम 0;
}

पूर्णाङ्क efi_get_variable_path(ध्रुव वर्ण *variable, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);

        r = efi_get_variable_string(variable, ret);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (ret)
                efi_tilt_backslashes(*ret);

        प्रत्यागम r;
}

स्थिर पूर्णाङ्क efi_verify_variable(ध्रुव वर्ण *variable, uint32_t attr, ध्रुव शून्य *मानम्, size_t size) {
        _cleanup_free_ शून्य *buf = रिक्त;
        size_t n;
        uint32_t a;
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);
        प्रतिज्ञा(मानम् || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम a == attr && memcmp_nn(buf, n, मानम्, size) == 0;
}

पूर्णाङ्क efi_set_variable(ध्रुव वर्ण *variable, ध्रुव शून्य *मानम्, size_t size) {
        स्थिर ध्रुव uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ संरचना var {
                uint32_t attr;
                वर्ण buf[];
        } _packed_ *buf = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);
        प्रतिज्ञा(मानम् || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        जइ (size > 0 && efi_verify_variable(variable, attr, मानम्, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                प्रत्यागम 0;
        }

        ध्रुव वर्ण *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ रिक्त,
                        /* flags= */ 0);
        जइ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        जइ (size == 0) {
                जइ (unlink(p) < 0) {
                        r = -errno;
                        गच्छ finish;
                }

                प्रत्यागम 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        जइ (fd < 0) {
                r = -errno;
                गच्छ finish;
        }

        buf = malloc(परिमाण(uint32_t) + size);
        जइ (!buf) {
                r = -ENOMEM;
                गच्छ finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, मानम्, size);

        r = loop_write(fd, buf, परिमाण(uint32_t) + size);
        जइ (r < 0)
                गच्छ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        जइ (futimens(fd, /* times= */ रिक्त) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        जइ (saved_flags_valid) {
                पूर्णाङ्क q;

                /* Restore the original flags field, just in case */
                जइ (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                अण्णहा
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                जइ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        प्रत्यागम r;
}

पूर्णाङ्क efi_set_variable_string(ध्रुव वर्ण *variable, ध्रुव वर्ण *मानम्) {
        _cleanup_free_ char16_t *u16 = रिक्त;

        u16 = utf8_to_utf16(मानम्, SIZE_MAX);
        जइ (!u16)
                प्रत्यागम -ENOMEM;

        प्रत्यागम efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * परिमाण(char16_t));
}

स्थिर पूर्णाङ्क cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        प्रत्यागम (cache_efi_boot = b);
}

bool is_efi_boot(शून्य) {
        जइ (cache_efi_boot >= 0)
                प्रत्यागम cache_efi_boot;

        जइ (detect_container() > 0)
                प्रत्यागम (cache_efi_boot = false);

        जइ (access("/sys/firmware/efi/", F_OK) < 0) {
                जइ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                प्रत्यागम (cache_efi_boot = false);
        }

        प्रत्यागम (cache_efi_boot = true);
}

स्थिर पूर्णाङ्क read_flag(ध्रुव वर्ण *variable) {
        _cleanup_free_ शून्य *v = रिक्त;
        uint8_t b;
        size_t s;
        पूर्णाङ्क r;

        जइ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                प्रत्यागम 0;

        r = efi_get_variable(variable, रिक्त, &v, &s);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (s != 1)
                प्रत्यागम -EINVAL;

        b = *(uint8_t *)v;
        प्रत्यागम !!b;
}

bool is_efi_secure_boot(शून्य) {
        स्थिर पूर्णाङ्क cache = -1;
        पूर्णाङ्क r;

        जइ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                जइ (r == -ENOENT)
                        cache = false;
                अण्णहा जइ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                अण्णहा
                        cache = r;
        }

        प्रत्यागम cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(शून्य) {
        स्थिर SecureBootMode cache = _SECURE_BOOT_INVALID;

        जइ (cache != _SECURE_BOOT_INVALID)
                प्रत्यागम cache;

        पूर्णाङ्क secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        जइ (secure < 0) {
                जइ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                प्रत्यागम (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        पूर्णाङ्क audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        पूर्णाङ्क deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        पूर्णाङ्क setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        पूर्णाङ्क moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        प्रत्यागम (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

वर्ण *efi_tilt_backslashes(वर्ण *s) {
        प्रत्यागम string_replace_char(s, '\\', '/');
}
