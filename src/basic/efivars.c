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

#यदि ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#परिभाषय EFI_N_RETRIES_NO_DELAY 20
#परिभाषय EFI_N_RETRIES_योग 25
#परिभाषय EFI_RETRY_DELAY (50 * USEC_प्रत्येकम्_MSEC)

पूर्णाङ्क efi_get_variable(
                ध्रुव वर्ण *variable,
                uint32_t *ret_attribute,
                शून्य **ret_मानम्,
                size_t *ret_आकार) {

        पूर्णाङ्क r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        प्रतिज्ञा(variable);

        ध्रुव वर्ण *p = strjoina("/sys/firmware/efi/efivars/", variable);

        यदि (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ पूर्णाङ्क fd = उद्घाटय(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        यदि (fd < 0)
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
        प्रति (अचिह्नित try = 0;; try++) {
                संरचना stat st;

                यदि (fstat(fd, &st) < 0)
                        प्रत्यागम log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                यदि (r < 0)
                        प्रत्यागम log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                यदि (st.st_size == 0) /* for uncommitted variables, see below */
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                यदि ((uint64_t) st.st_size < परिमाण(attr))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, परिमाण(attr));
                यदि ((uint64_t) st.st_size > परिमाण(attr) + 4 * U64_MB)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                यदि (!ret_attribute && !ret_मानम्) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        भङ्ग;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_आकार = (size_t) st.st_size, payload_आकार, read_आकार, alloc_आकार, read_सीमा;
                यदि (!SUB_SAFE(&payload_आकार, file_आकार, परिमाण(attr)) ||
                    !सङ्कलय_SAFE(&read_आकार, payload_आकार, 1) ||
                    !सङ्कलय_SAFE(&alloc_आकार, read_आकार, 3) ||
                    !सङ्कलय_SAFE(&read_सीमा, file_आकार, 1))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                मुक्त(buf);
                buf = स्मृतिं_देहि(alloc_आकार);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                संरचना iovec iov[] = {
                        { &attr, परिमाण(attr)                           },
                        { buf,   read_आकार                              },
                };

                n = readv(fd, iov, 2);
                यदि (n < 0) {
                        यदि (errno != EINTR)
                                प्रत्यागम log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } अन्यथा यदि ((size_t) n == read_सीमा)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                अन्यथा {
                        प्रतिज्ञा((size_t) n < read_सीमा);
                        भङ्ग;
                }

                यदि (try >= EFI_N_RETRIES_योग)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                यदि (try >= EFI_N_RETRIES_NO_DELAY)
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
        यदि (n == 0)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        यदि ((size_t) n < परिमाण(attr))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, परिमाण(attr));
        size_t मानम्_आकार = n - परिमाण(attr);

        यदि (ret_attribute)
                *ret_attribute = attr;

        यदि (ret_मानम्) {
                प्रतिज्ञा(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[मानम्_आकार] = 0;
                buf[मानम्_आकार + 1] = 0;
                buf[मानम्_आकार + 2] = 0;
                *ret_मानम् = TAKE_PTR(buf);
        }

        यदि (DEBUG_LOGGING) {
                usec_t अन्तः = now(CLOCK_MONOTONIC);
                यदि (अन्तः > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(अन्तः - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        यदि (ret_आकार)
                *ret_आकार = मानम्_आकार;

        प्रत्यागम 0;
}

पूर्णाङ्क efi_get_variable_string(ध्रुव वर्ण *variable, वर्ण **ret) {
        _cleanup_free_ शून्य *s = रिक्त, *x = रिक्त;
        size_t ss = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);

        r = efi_get_variable(variable, रिक्त, &s, &ss);
        यदि (r < 0)
                प्रत्यागम r;

        x = utf16_गन्तव्यम्_utf8(s, ss);
        यदि (!x)
                प्रत्यागम -ENOMEM;

        यदि (ret)
                *ret = TAKE_PTR(x);

        प्रत्यागम 0;
}

पूर्णाङ्क efi_get_variable_मार्गः(ध्रुव वर्ण *variable, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);

        r = efi_get_variable_string(variable, ret);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret)
                efi_tilt_backslashes(*ret);

        प्रत्यागम r;
}

स्थिर पूर्णाङ्क efi_verify_variable(ध्रुव वर्ण *variable, uint32_t attr, ध्रुव शून्य *मानम्, size_t आकार) {
        _cleanup_free_ शून्य *buf = रिक्त;
        size_t n;
        uint32_t a;
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);
        प्रतिज्ञा(मानम् || आकार == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम a == attr && memcmp_nn(buf, n, मानम्, आकार) == 0;
}

पूर्णाङ्क efi_समुच्चयः_variable(ध्रुव वर्ण *variable, ध्रुव शून्य *मानम्, size_t आकार) {
        स्थिर ध्रुव uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ संरचना var {
                uint32_t attr;
                वर्ण buf[];
        } _packed_ *buf = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        bool saved_flags_valid = false;
        अचिह्नित saved_flags;
        पूर्णाङ्क r;

        प्रतिज्ञा(variable);
        प्रतिज्ञा(मानम् || आकार == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        यदि (आकार > 0 && efi_verify_variable(variable, attr, मानम्, आकार) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                प्रत्यागम 0;
        }

        ध्रुव वर्ण *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_पूर्णम्(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ रिक्त,
                        /* flags= */ 0);
        यदि (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        यदि (आकार == 0) {
                यदि (unlink(p) < 0) {
                        r = -errno;
                        गच्छ finish;
                }

                प्रत्यागम 0;
        }

        fd = उद्घाटय(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        यदि (fd < 0) {
                r = -errno;
                गच्छ finish;
        }

        buf = स्मृतिं_देहि(परिमाण(uint32_t) + आकार);
        यदि (!buf) {
                r = -ENOMEM;
                गच्छ finish;
        }

        buf->attr = attr;
        स्मृतिं_प्रतिलिख(buf->buf, मानम्, आकार);

        r = loop_write(fd, buf, परिमाण(uint32_t) + आकार);
        यदि (r < 0)
                गच्छ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        यदि (futimens(fd, /* times= */ रिक्त) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        यदि (saved_flags_valid) {
                पूर्णाङ्क q;

                /* Restore the original flags field, just in case */
                यदि (fd < 0)
                        q = chattr_मार्गः(p, saved_flags, FS_IMMUTABLE_FL);
                अन्यथा
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                यदि (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        प्रत्यागम r;
}

पूर्णाङ्क efi_समुच्चयः_variable_string(ध्रुव वर्ण *variable, ध्रुव वर्ण *मानम्) {
        _cleanup_free_ char16_t *u16 = रिक्त;

        u16 = utf8_गन्तव्यम्_utf16(मानम्, SIZE_MAX);
        यदि (!u16)
                प्रत्यागम -ENOMEM;

        प्रत्यागम efi_समुच्चयः_variable(variable, u16, (char16_strlen(u16) + 1) * परिमाण(char16_t));
}

स्थिर पूर्णाङ्क cache_efi_boot = -1;

bool समुच्चयः_efi_boot(bool b) {
        प्रत्यागम (cache_efi_boot = b);
}

bool is_efi_boot(शून्य) {
        यदि (cache_efi_boot >= 0)
                प्रत्यागम cache_efi_boot;

        यदि (detect_container() > 0)
                प्रत्यागम (cache_efi_boot = false);

        यदि (access("/sys/firmware/efi/", F_OK) < 0) {
                यदि (errno != ENOENT)
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

        यदि (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                प्रत्यागम 0;

        r = efi_get_variable(variable, रिक्त, &v, &s);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (s != 1)
                प्रत्यागम -EINVAL;

        b = *(uint8_t *)v;
        प्रत्यागम !!b;
}

bool is_efi_secure_boot(शून्य) {
        स्थिर पूर्णाङ्क cache = -1;
        पूर्णाङ्क r;

        यदि (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                यदि (r == -ENOENT)
                        cache = false;
                अन्यथा यदि (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                अन्यथा
                        cache = r;
        }

        प्रत्यागम cache > 0;
}

Secure_Boot_बहुलकम् efi_get_secure_boot_बहुलकम्(शून्य) {
        स्थिर Secure_Boot_बहुलकम् cache = _SECURE_BOOT_INVALID;

        यदि (cache != _SECURE_BOOT_INVALID)
                प्रत्यागम cache;

        पूर्णाङ्क secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        यदि (secure < 0) {
                यदि (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                प्रत्यागम (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        पूर्णाङ्क audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        पूर्णाङ्क deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        पूर्णाङ्क सज्जता    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        पूर्णाङ्क moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, सज्जता, moksb);

        प्रत्यागम (cache = decode_secure_boot_बहुलकम्(secure, audit > 0, deployed > 0, सज्जता > 0, moksb > 0));
}
#यदि_अन्तः

वर्ण *efi_tilt_backslashes(वर्ण *s) {
        प्रत्यागम string_replace_char(s, '\\', '/');
}
