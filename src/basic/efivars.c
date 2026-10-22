/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <sys/uio.h>
#অন্তর্ভাৱ <time.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "chattr-util.h"
#অন্তর্ভাৱ "efivars.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "io-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "memory-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "time-util.h"
#অন্তর্ভাৱ "utf8.h"
#অন্তর্ভাৱ "virt.h"

#যদি ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

পূর্ণাঙ্ক efi_get_variable(
                ধ্রুৱ ৱর্ণ *variable,
                uint32_t *ret_attribute,
                শূন্য **ret_মান,
                size_t *ret_size) {

        পূর্ণাঙ্ক r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        প্রতিজ্ঞা(variable);

        ধ্রুৱ ৱর্ণ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        যদি (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ পূর্ণাঙ্ক fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                ঘূৰাই log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ ৱর্ণ *buf = রিক্ত;
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
        বাবে (unsigned try = 0;; try++) {
                সংরচনা stat st;

                যদি (fstat(fd, &st) < 0)
                        ঘূৰাই log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                যদি (r < 0)
                        ঘূৰাই log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                যদি (st.st_size == 0) /* for uncommitted variables, see below */
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                যদি ((uint64_t) st.st_size < পরিমাণ(attr))
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, পরিমাণ(attr));
                যদি ((uint64_t) st.st_size > পরিমাণ(attr) + 4 * U64_MB)
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                যদি (!ret_attribute && !ret_মান) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        ভঙ্গ;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                যদি (!SUB_SAFE(&payload_size, file_size, পরিমাণ(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                মুক্ত(buf);
                buf = malloc(alloc_size);
                যদি (!buf)
                        ঘূৰাই -ENOMEM;

                সংরচনা iovec iov[] = {
                        { &attr, পরিমাণ(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                যদি (n < 0) {
                        যদি (errno != EINTR)
                                ঘূৰাই log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } অন্যথা যদি ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                অন্যথা {
                        প্রতিজ্ঞা((size_t) n < read_limit);
                        ভঙ্গ;
                }

                যদি (try >= EFI_N_RETRIES_TOTAL)
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
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
                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        যদি ((size_t) n < পরিমাণ(attr))
                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, পরিমাণ(attr));
        size_t মান_size = n - পরিমাণ(attr);

        যদি (ret_attribute)
                *ret_attribute = attr;

        যদি (ret_মান) {
                প্রতিজ্ঞা(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[মান_size] = 0;
                buf[মান_size + 1] = 0;
                buf[মান_size + 2] = 0;
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

        যদি (ret_size)
                *ret_size = মান_size;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক efi_get_variable_string(ধ্রুৱ ৱর্ণ *variable, ৱর্ণ **ret) {
        _cleanup_free_ শূন্য *s = রিক্ত, *x = রিক্ত;
        size_t ss = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(variable);

        r = efi_get_variable(variable, রিক্ত, &s, &ss);
        যদি (r < 0)
                ঘূৰাই r;

        x = utf16_to_utf8(s, ss);
        যদি (!x)
                ঘূৰাই -ENOMEM;

        যদি (ret)
                *ret = TAKE_PTR(x);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক efi_get_variable_path(ধ্রুৱ ৱর্ণ *variable, ৱর্ণ **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(variable);

        r = efi_get_variable_string(variable, ret);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ret)
                efi_tilt_backslashes(*ret);

        ঘূৰাই r;
}

স্থির পূর্ণাঙ্ক efi_verify_variable(ধ্রুৱ ৱর্ণ *variable, uint32_t attr, ধ্রুৱ শূন্য *মান, size_t size) {
        _cleanup_free_ শূন্য *buf = রিক্ত;
        size_t n;
        uint32_t a;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(variable);
        প্রতিজ্ঞা(মান || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই a == attr && memcmp_nn(buf, n, মান, size) == 0;
}

পূর্ণাঙ্ক efi_set_variable(ধ্রুৱ ৱর্ণ *variable, ধ্রুৱ শূন্য *মান, size_t size) {
        স্থির ধ্রুৱ uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ সংরচনা var {
                uint32_t attr;
                ৱর্ণ buf[];
        } _packed_ *buf = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(variable);
        প্রতিজ্ঞা(মান || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        যদি (size > 0 && efi_verify_variable(variable, attr, মান, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ঘূৰাই 0;
        }

        ধ্রুৱ ৱর্ণ *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ রিক্ত,
                        /* flags= */ 0);
        যদি (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        যদি (size == 0) {
                যদি (unlink(p) < 0) {
                        r = -errno;
                        গচ্ছ finish;
                }

                ঘূৰাই 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        যদি (fd < 0) {
                r = -errno;
                গচ্ছ finish;
        }

        buf = malloc(পরিমাণ(uint32_t) + size);
        যদি (!buf) {
                r = -ENOMEM;
                গচ্ছ finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, মান, size);

        r = loop_write(fd, buf, পরিমাণ(uint32_t) + size);
        যদি (r < 0)
                গচ্ছ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        যদি (futimens(fd, /* times= */ রিক্ত) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        যদি (saved_flags_valid) {
                পূর্ণাঙ্ক q;

                /* Restore the original flags field, just in case */
                যদি (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                অন্যথা
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                যদি (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ঘূৰাই r;
}

পূর্ণাঙ্ক efi_set_variable_string(ধ্রুৱ ৱর্ণ *variable, ধ্রুৱ ৱর্ণ *মান) {
        _cleanup_free_ char16_t *u16 = রিক্ত;

        u16 = utf8_to_utf16(মান, SIZE_MAX);
        যদি (!u16)
                ঘূৰাই -ENOMEM;

        ঘূৰাই efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * পরিমাণ(char16_t));
}

স্থির পূর্ণাঙ্ক cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        ঘূৰাই (cache_efi_boot = b);
}

bool is_efi_boot(শূন্য) {
        যদি (cache_efi_boot >= 0)
                ঘূৰাই cache_efi_boot;

        যদি (detect_container() > 0)
                ঘূৰাই (cache_efi_boot = false);

        যদি (access("/sys/firmware/efi/", F_OK) < 0) {
                যদি (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ঘূৰাই (cache_efi_boot = false);
        }

        ঘূৰাই (cache_efi_boot = true);
}

স্থির পূর্ণাঙ্ক read_flag(ধ্রুৱ ৱর্ণ *variable) {
        _cleanup_free_ শূন্য *v = রিক্ত;
        uint8_t b;
        size_t s;
        পূর্ণাঙ্ক r;

        যদি (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ঘূৰাই 0;

        r = efi_get_variable(variable, রিক্ত, &v, &s);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (s != 1)
                ঘূৰাই -EINVAL;

        b = *(uint8_t *)v;
        ঘূৰাই !!b;
}

bool is_efi_secure_boot(শূন্য) {
        স্থির পূর্ণাঙ্ক cache = -1;
        পূর্ণাঙ্ক r;

        যদি (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                যদি (r == -ENOENT)
                        cache = false;
                অন্যথা যদি (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                অন্যথা
                        cache = r;
        }

        ঘূৰাই cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(শূন্য) {
        স্থির SecureBootMode cache = _SECURE_BOOT_INVALID;

        যদি (cache != _SECURE_BOOT_INVALID)
                ঘূৰাই cache;

        পূর্ণাঙ্ক secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        যদি (secure < 0) {
                যদি (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ঘূৰাই (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        পূর্ণাঙ্ক audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        পূর্ণাঙ্ক deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        পূর্ণাঙ্ক setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        পূর্ণাঙ্ক moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        ঘূৰাই (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

ৱর্ণ *efi_tilt_backslashes(ৱর্ণ *s) {
        ঘূৰাই string_replace_char(s, '\\', '/');
}
