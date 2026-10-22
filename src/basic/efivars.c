/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <sys/uio.h>
#অন্তর্ভাব <time.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chattr-util.h"
#অন্তর্ভাব "efivars.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "utf8.h"
#অন্তর্ভাব "virt.h"

#যদি ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_যোগ 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

পূর্ণসংখ্যা efi_get_variable(
                ধ্রুবক অক্ষর *variable,
                uint32_t *ret_attribute,
                শূন্য **ret_মান,
                size_t *ret_আকার) {

        পূর্ণসংখ্যা r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        প্রতিজ্ঞা(variable);

        ধ্রুবক অক্ষর *p = strjoina("/sys/firmware/efi/efivars/", variable);

        যদি (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ পূর্ণসংখ্যা fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ অক্ষর *buf = ফাঁকা;
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
        জন্য (unsigned try = 0;; try++) {
                কাঠামো stat st;

                যদি (fstat(fd, &st) < 0)
                        ফেরত log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                যদি (r < 0)
                        ফেরত log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                যদি (st.st_size == 0) /* for uncommitted variables, see below */
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                যদি ((uint64_t) st.st_size < আকার(attr))
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, আকার(attr));
                যদি ((uint64_t) st.st_size > আকার(attr) + 4 * U64_MB)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                যদি (!ret_attribute && !ret_মান) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        বিরতি;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_আকার = (size_t) st.st_size, payload_আকার, read_আকার, alloc_আকার, read_limit;
                যদি (!SUB_SAFE(&payload_আকার, file_আকার, আকার(attr)) ||
                    !ADD_SAFE(&read_আকার, payload_আকার, 1) ||
                    !ADD_SAFE(&alloc_আকার, read_আকার, 3) ||
                    !ADD_SAFE(&read_limit, file_আকার, 1))
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                মুক্ত(buf);
                buf = malloc(alloc_আকার);
                যদি (!buf)
                        ফেরত -ENOMEM;

                কাঠামো iovec iov[] = {
                        { &attr, আকার(attr)                           },
                        { buf,   read_আকার                              },
                };

                n = readv(fd, iov, 2);
                যদি (n < 0) {
                        যদি (errno != EINTR)
                                ফেরত log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } অন্যথা যদি ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                অন্যথা {
                        প্রতিজ্ঞা((size_t) n < read_limit);
                        বিরতি;
                }

                যদি (try >= EFI_N_RETRIES_যোগ)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                যদি (try >= EFI_N_RETRIES_NO_DELAY)
                        (শূন্য) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (শূন্য) lseek(fd, 0, SEEK_SET);
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
        যদি (n == 0)
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        যদি ((size_t) n < আকার(attr))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, আকার(attr));
        size_t মান_আকার = n - আকার(attr);

        যদি (ret_attribute)
                *ret_attribute = attr;

        যদি (ret_মান) {
                প্রতিজ্ঞা(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[মান_আকার] = 0;
                buf[মান_আকার + 1] = 0;
                buf[মান_আকার + 2] = 0;
                *ret_মান = TAKE_PTR(buf);
        }

        যদি (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                যদি (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        যদি (ret_আকার)
                *ret_আকার = মান_আকার;

        ফেরত 0;
}

পূর্ণসংখ্যা efi_get_variable_string(ধ্রুবক অক্ষর *variable, অক্ষর **ret) {
        _cleanup_free_ শূন্য *s = ফাঁকা, *x = ফাঁকা;
        size_t ss = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(variable);

        r = efi_get_variable(variable, ফাঁকা, &s, &ss);
        যদি (r < 0)
                ফেরত r;

        x = utf16_to_utf8(s, ss);
        যদি (!x)
                ফেরত -ENOMEM;

        যদি (ret)
                *ret = TAKE_PTR(x);

        ফেরত 0;
}

পূর্ণসংখ্যা efi_get_variable_path(ধ্রুবক অক্ষর *variable, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(variable);

        r = efi_get_variable_string(variable, ret);
        যদি (r < 0)
                ফেরত r;

        যদি (ret)
                efi_tilt_backslashes(*ret);

        ফেরত r;
}

স্থির পূর্ণসংখ্যা efi_verify_variable(ধ্রুবক অক্ষর *variable, uint32_t attr, ধ্রুবক শূন্য *মান, size_t আকার_2) {
        _cleanup_free_ শূন্য *buf = ফাঁকা;
        size_t n;
        uint32_t a;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(variable);
        প্রতিজ্ঞা(মান || আকার_2 == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        যদি (r < 0)
                ফেরত r;

        ফেরত a == attr && memcmp_nn(buf, n, মান, আকার_2) == 0;
}

পূর্ণসংখ্যা efi_set_variable(ধ্রুবক অক্ষর *variable, ধ্রুবক শূন্য *মান, size_t আকার_2) {
        স্থির ধ্রুবক uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ কাঠামো var {
                uint32_t attr;
                অক্ষর buf[];
        } _packed_ *buf = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(variable);
        প্রতিজ্ঞা(মান || আকার_2 == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        যদি (আকার_2 > 0 && efi_verify_variable(variable, attr, মান, আকার_2) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ফেরত 0;
        }

        ধ্রুবক অক্ষর *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ফাঁকা,
                        /* flags= */ 0);
        যদি (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        যদি (আকার_2 == 0) {
                যদি (unlink(p) < 0) {
                        r = -errno;
                        যাও finish;
                }

                ফেরত 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        যদি (fd < 0) {
                r = -errno;
                যাও finish;
        }

        buf = malloc(আকার(uint32_t) + আকার_2);
        যদি (!buf) {
                r = -ENOMEM;
                যাও finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, মান, আকার_2);

        r = loop_write(fd, buf, আকার(uint32_t) + আকার_2);
        যদি (r < 0)
                যাও finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        যদি (futimens(fd, /* times= */ ফাঁকা) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        যদি (saved_flags_valid) {
                পূর্ণসংখ্যা q;

                /* Restore the original flags field, just in case */
                যদি (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                অন্যথা
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                যদি (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ফেরত r;
}

পূর্ণসংখ্যা efi_set_variable_string(ধ্রুবক অক্ষর *variable, ধ্রুবক অক্ষর *মান) {
        _cleanup_free_ char16_t *u16 = ফাঁকা;

        u16 = utf8_to_utf16(মান, SIZE_MAX);
        যদি (!u16)
                ফেরত -ENOMEM;

        ফেরত efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * আকার(char16_t));
}

স্থির পূর্ণসংখ্যা cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        ফেরত (cache_efi_boot = b);
}

bool is_efi_boot(শূন্য) {
        যদি (cache_efi_boot >= 0)
                ফেরত cache_efi_boot;

        যদি (detect_container() > 0)
                ফেরত (cache_efi_boot = false);

        যদি (access("/sys/firmware/efi/", F_OK) < 0) {
                যদি (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ফেরত (cache_efi_boot = false);
        }

        ফেরত (cache_efi_boot = true);
}

স্থির পূর্ণসংখ্যা read_flag(ধ্রুবক অক্ষর *variable) {
        _cleanup_free_ শূন্য *v = ফাঁকা;
        uint8_t b;
        size_t s;
        পূর্ণসংখ্যা r;

        যদি (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ফেরত 0;

        r = efi_get_variable(variable, ফাঁকা, &v, &s);
        যদি (r < 0)
                ফেরত r;

        যদি (s != 1)
                ফেরত -EINVAL;

        b = *(uint8_t *)v;
        ফেরত !!b;
}

bool is_efi_secure_boot(শূন্য) {
        স্থির পূর্ণসংখ্যা cache = -1;
        পূর্ণসংখ্যা r;

        যদি (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                যদি (r == -ENOENT)
                        cache = false;
                অন্যথা যদি (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                অন্যথা
                        cache = r;
        }

        ফেরত cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(শূন্য) {
        স্থির SecureBootMode cache = _SECURE_BOOT_INVALID;

        যদি (cache != _SECURE_BOOT_INVALID)
                ফেরত cache;

        পূর্ণসংখ্যা secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        যদি (secure < 0) {
                যদি (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ফেরত (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        পূর্ণসংখ্যা audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        পূর্ণসংখ্যা deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        পূর্ণসংখ্যা setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        পূর্ণসংখ্যা moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        ফেরত (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

অক্ষর *efi_tilt_backslashes(অক্ষর *s) {
        ফেরত string_replace_char(s, '\\', '/');
}
