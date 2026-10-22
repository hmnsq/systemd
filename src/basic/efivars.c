/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <sys/uio.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chattr-util.h"
#అన్తర్భావ "efivars.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "utf8.h"
#అన్తర్భావ "virt.h"

#ఒకవేళ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#నిర్వచించు EFI_N_RETRIES_NO_DELAY 20
#నిర్వచించు EFI_N_RETRIES_మొత్తం 25
#నిర్వచించు EFI_RETRY_DELAY (50 * USEC_చొప్పున_MSEC)

పూర్ణాంకం efi_get_variable(
                స్థిరరాశి అక్షరం *variable,
                uint32_t *ret_attribute,
                శూన్య **ret_విలువ,
                size_t *ret_కొలత) {

        పూర్ణాంకం r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        ప్రతిజ్ఞా(variable);

        స్థిరరాశి అక్షరం *p = strjoina("/sys/firmware/efi/efivars/", variable);

        ఒకవేళ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ పూర్ణాంకం fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ అక్షరం *buf = ఖాళీ;
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
        కొరకు (గుర్తులేని try = 0;; try++) {
                నిర్మాణం stat st;

                ఒకవేళ (fstat(fd, &st) < 0)
                        తిరిగి log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                ఒకవేళ (st.st_size == 0) /* for uncommitted variables, see below */
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                ఒకవేళ ((uint64_t) st.st_size < పరిమాణం(attr))
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, పరిమాణం(attr));
                ఒకవేళ ((uint64_t) st.st_size > పరిమాణం(attr) + 4 * U64_MB)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                ఒకవేళ (!ret_attribute && !ret_విలువ) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        ఆపు;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_కొలత = (size_t) st.st_size, payload_కొలత, read_కొలత, alloc_కొలత, read_హద్దు;
                ఒకవేళ (!SUB_సురక్షిత(&payload_కొలత, file_కొలత, పరిమాణం(attr)) ||
                    !కూడు_సురక్షిత(&read_కొలత, payload_కొలత, 1) ||
                    !కూడు_సురక్షిత(&alloc_కొలత, read_కొలత, 3) ||
                    !కూడు_సురక్షిత(&read_హద్దు, file_కొలత, 1))
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                విడుదల(buf);
                buf = జ్ఞాపకం_ఇవ్వు(alloc_కొలత);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                నిర్మాణం iovec iov[] = {
                        { &attr, పరిమాణం(attr)                           },
                        { buf,   read_కొలత                              },
                };

                n = readv(fd, iov, 2);
                ఒకవేళ (n < 0) {
                        ఒకవేళ (errno != EINTR)
                                తిరిగి log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } లేకపోతే ఒకవేళ ((size_t) n == read_హద్దు)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                లేకపోతే {
                        ప్రతిజ్ఞా((size_t) n < read_హద్దు);
                        ఆపు;
                }

                ఒకవేళ (try >= EFI_N_RETRIES_మొత్తం)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                ఒకవేళ (try >= EFI_N_RETRIES_NO_DELAY)
                        (శూన్య) usleep_సురక్షిత(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (శూన్య) lseek(fd, 0, SEEK_SET);
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
        ఒకవేళ (n == 0)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        ఒకవేళ ((size_t) n < పరిమాణం(attr))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, పరిమాణం(attr));
        size_t విలువ_కొలత = n - పరిమాణం(attr);

        ఒకవేళ (ret_attribute)
                *ret_attribute = attr;

        ఒకవేళ (ret_విలువ) {
                ప్రతిజ్ఞా(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[విలువ_కొలత] = 0;
                buf[విలువ_కొలత + 1] = 0;
                buf[విలువ_కొలత + 2] = 0;
                *ret_విలువ = TAKE_PTR(buf);
        }

        ఒకవేళ (DEBUG_LOGGING) {
                usec_t చివర = now(CLOCK_MONOTONIC);
                ఒకవేళ (చివర > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(చివర - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        ఒకవేళ (ret_కొలత)
                *ret_కొలత = విలువ_కొలత;

        తిరిగి 0;
}

పూర్ణాంకం efi_get_variable_string(స్థిరరాశి అక్షరం *variable, అక్షరం **ret) {
        _cleanup_free_ శూన్య *s = ఖాళీ, *x = ఖాళీ;
        size_t ss = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(variable);

        r = efi_get_variable(variable, ఖాళీ, &s, &ss);
        ఒకవేళ (r < 0)
                తిరిగి r;

        x = utf16_గమ్యం_utf8(s, ss);
        ఒకవేళ (!x)
                తిరిగి -ENOMEM;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(x);

        తిరిగి 0;
}

పూర్ణాంకం efi_get_variable_మార్గం(స్థిరరాశి అక్షరం *variable, అక్షరం **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(variable);

        r = efi_get_variable_string(variable, ret);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                efi_tilt_backslashes(*ret);

        తిరిగి r;
}

స్థిర పూర్ణాంకం efi_verify_variable(స్థిరరాశి అక్షరం *variable, uint32_t attr, స్థిరరాశి శూన్య *విలువ, size_t కొలత) {
        _cleanup_free_ శూన్య *buf = ఖాళీ;
        size_t n;
        uint32_t a;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(variable);
        ప్రతిజ్ఞా(విలువ || కొలత == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి a == attr && memcmp_nn(buf, n, విలువ, కొలత) == 0;
}

పూర్ణాంకం efi_సమితి_variable(స్థిరరాశి అక్షరం *variable, స్థిరరాశి శూన్య *విలువ, size_t కొలత) {
        స్థిర స్థిరరాశి uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ నిర్మాణం var {
                uint32_t attr;
                అక్షరం buf[];
        } _packed_ *buf = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        bool saved_flags_valid = false;
        గుర్తులేని saved_flags;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(variable);
        ప్రతిజ్ఞా(విలువ || కొలత == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        ఒకవేళ (కొలత > 0 && efi_verify_variable(variable, attr, విలువ, కొలత) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                తిరిగి 0;
        }

        స్థిరరాశి అక్షరం *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_నిండిన(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ఖాళీ,
                        /* flags= */ 0);
        ఒకవేళ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        ఒకవేళ (కొలత == 0) {
                ఒకవేళ (unlink(p) < 0) {
                        r = -errno;
                        వెళ్ళు ముగింపు;
                }

                తిరిగి 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        ఒకవేళ (fd < 0) {
                r = -errno;
                వెళ్ళు ముగింపు;
        }

        buf = జ్ఞాపకం_ఇవ్వు(పరిమాణం(uint32_t) + కొలత);
        ఒకవేళ (!buf) {
                r = -ENOMEM;
                వెళ్ళు ముగింపు;
        }

        buf->attr = attr;
        జ్ఞాపకం_నకలు(buf->buf, విలువ, కొలత);

        r = loop_write(fd, buf, పరిమాణం(uint32_t) + కొలత);
        ఒకవేళ (r < 0)
                వెళ్ళు ముగింపు;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        ఒకవేళ (futimens(fd, /* times= */ ఖాళీ) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

ముగింపు:
        ఒకవేళ (saved_flags_valid) {
                పూర్ణాంకం q;

                /* Restore the original flags field, just in case */
                ఒకవేళ (fd < 0)
                        q = chattr_మార్గం(p, saved_flags, FS_IMMUTABLE_FL);
                లేకపోతే
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                ఒకవేళ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        తిరిగి r;
}

పూర్ణాంకం efi_సమితి_variable_string(స్థిరరాశి అక్షరం *variable, స్థిరరాశి అక్షరం *విలువ) {
        _cleanup_free_ char16_t *u16 = ఖాళీ;

        u16 = utf8_గమ్యం_utf16(విలువ, SIZE_MAX);
        ఒకవేళ (!u16)
                తిరిగి -ENOMEM;

        తిరిగి efi_సమితి_variable(variable, u16, (char16_strlen(u16) + 1) * పరిమాణం(char16_t));
}

స్థిర పూర్ణాంకం cache_efi_boot = -1;

bool సమితి_efi_boot(bool b) {
        తిరిగి (cache_efi_boot = b);
}

bool is_efi_boot(శూన్య) {
        ఒకవేళ (cache_efi_boot >= 0)
                తిరిగి cache_efi_boot;

        ఒకవేళ (detect_container() > 0)
                తిరిగి (cache_efi_boot = false);

        ఒకవేళ (access("/sys/firmware/efi/", F_OK) < 0) {
                ఒకవేళ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                తిరిగి (cache_efi_boot = false);
        }

        తిరిగి (cache_efi_boot = true);
}

స్థిర పూర్ణాంకం read_flag(స్థిరరాశి అక్షరం *variable) {
        _cleanup_free_ శూన్య *v = ఖాళీ;
        uint8_t b;
        size_t s;
        పూర్ణాంకం r;

        ఒకవేళ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                తిరిగి 0;

        r = efi_get_variable(variable, ఖాళీ, &v, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (s != 1)
                తిరిగి -EINVAL;

        b = *(uint8_t *)v;
        తిరిగి !!b;
}

bool is_efi_secure_boot(శూన్య) {
        స్థిర పూర్ణాంకం cache = -1;
        పూర్ణాంకం r;

        ఒకవేళ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                ఒకవేళ (r == -ENOENT)
                        cache = false;
                లేకపోతే ఒకవేళ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                లేకపోతే
                        cache = r;
        }

        తిరిగి cache > 0;
}

Secure_Boot_బహుళకం efi_get_secure_boot_బహుళకం(శూన్య) {
        స్థిర Secure_Boot_బహుళకం cache = _SECURE_BOOT_INVALID;

        ఒకవేళ (cache != _SECURE_BOOT_INVALID)
                తిరిగి cache;

        పూర్ణాంకం secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        ఒకవేళ (secure < 0) {
                ఒకవేళ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                తిరిగి (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        పూర్ణాంకం audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        పూర్ణాంకం deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        పూర్ణాంకం అమరిక    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        పూర్ణాంకం moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, అమరిక, moksb);

        తిరిగి (cache = decode_secure_boot_బహుళకం(secure, audit > 0, deployed > 0, అమరిక > 0, moksb > 0));
}
#అయితే_ముగింపు

అక్షరం *efi_tilt_backslashes(అక్షరం *s) {
        తిరిగి string_replace_char(s, '\\', '/');
}
