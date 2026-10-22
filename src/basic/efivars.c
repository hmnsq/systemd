/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <sys/uio.h>
#ଅନ୍ତର୍ଭାଵ <time.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chattr-util.h"
#ଅନ୍ତର୍ଭାଵ "efivars.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"
#ଅନ୍ତର୍ଭାଵ "virt.h"

#ଯଦି ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା efi_get_variable(
                ଅଚଳ ଅକ୍ଷର *variable,
                uint32_t *ret_attribute,
                ଶୂନ୍ୟ **ret_ମୂଲ୍ୟ,
                size_t *ret_size) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        ପ୍ରତିଜ୍ଞା(variable);

        ଅଚଳ ଅକ୍ଷର *p = strjoina("/sys/firmware/efi/efivars/", variable);

        ଯଦି (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
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
        ପାଇଁ (unsigned try = 0;; try++) {
                ସଂରଚନା stat st;

                ଯଦି (fstat(fd, &st) < 0)
                        ଫେରନ୍ତୁ log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                ଯଦି (st.st_size == 0) /* for uncommitted variables, see below */
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                ଯଦି ((uint64_t) st.st_size < ଆକାର(attr))
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, ଆକାର(attr));
                ଯଦି ((uint64_t) st.st_size > ଆକାର(attr) + 4 * U64_MB)
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                ଯଦି (!ret_attribute && !ret_ମୂଲ୍ୟ) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        ଭାଙ୍ଗ;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                ଯଦି (!SUB_SAFE(&payload_size, file_size, ଆକାର(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                ମୁକ୍ତ(buf);
                buf = malloc(alloc_size);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                ସଂରଚନା iovec iov[] = {
                        { &attr, ଆକାର(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                ଯଦି (n < 0) {
                        ଯଦି (errno != EINTR)
                                ଫେରନ୍ତୁ log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } ଅନ୍ୟଥା ଯଦି ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                ଅନ୍ୟଥା {
                        ପ୍ରତିଜ୍ଞା((size_t) n < read_limit);
                        ଭାଙ୍ଗ;
                }

                ଯଦି (try >= EFI_N_RETRIES_TOTAL)
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                ଯଦି (try >= EFI_N_RETRIES_NO_DELAY)
                        (ଶୂନ୍ୟ) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (ଶୂନ୍ୟ) lseek(fd, 0, SEEK_SET);
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
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        ଯଦି ((size_t) n < ଆକାର(attr))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, ଆକାର(attr));
        size_t ମୂଲ୍ୟ_size = n - ଆକାର(attr);

        ଯଦି (ret_attribute)
                *ret_attribute = attr;

        ଯଦି (ret_ମୂଲ୍ୟ) {
                ପ୍ରତିଜ୍ଞା(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[ମୂଲ୍ୟ_size] = 0;
                buf[ମୂଲ୍ୟ_size + 1] = 0;
                buf[ମୂଲ୍ୟ_size + 2] = 0;
                *ret_ମୂଲ୍ୟ = TAKE_PTR(buf);
        }

        ଯଦି (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                ଯଦି (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        ଯଦି (ret_size)
                *ret_size = ମୂଲ୍ୟ_size;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା efi_get_variable_string(ଅଚଳ ଅକ୍ଷର *variable, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଶୂନ୍ୟ *s = ଖାଲି, *x = ଖାଲି;
        size_t ss = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(variable);

        r = efi_get_variable(variable, ଖାଲି, &s, &ss);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        x = utf16_to_utf8(s, ss);
        ଯଦି (!x)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (ret)
                *ret = TAKE_PTR(x);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା efi_get_variable_path(ଅଚଳ ଅକ୍ଷର *variable, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(variable);

        r = efi_get_variable_string(variable, ret);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                efi_tilt_backslashes(*ret);

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା efi_verify_variable(ଅଚଳ ଅକ୍ଷର *variable, uint32_t attr, ଅଚଳ ଶୂନ୍ୟ *ମୂଲ୍ୟ, size_t size) {
        _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;
        size_t n;
        uint32_t a;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(variable);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ a == attr && memcmp_nn(buf, n, ମୂଲ୍ୟ, size) == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା efi_set_variable(ଅଚଳ ଅକ୍ଷର *variable, ଅଚଳ ଶୂନ୍ୟ *ମୂଲ୍ୟ, size_t size) {
        ସ୍ଥିର ଅଚଳ uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ସଂରଚନା var {
                uint32_t attr;
                ଅକ୍ଷର buf[];
        } _packed_ *buf = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(variable);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        ଯଦି (size > 0 && efi_verify_variable(variable, attr, ମୂଲ୍ୟ, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ଫେରନ୍ତୁ 0;
        }

        ଅଚଳ ଅକ୍ଷର *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ଖାଲି,
                        /* flags= */ 0);
        ଯଦି (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        ଯଦି (size == 0) {
                ଯଦି (unlink(p) < 0) {
                        r = -errno;
                        ଯାଅ finish;
                }

                ଫେରନ୍ତୁ 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        ଯଦି (fd < 0) {
                r = -errno;
                ଯାଅ finish;
        }

        buf = malloc(ଆକାର(uint32_t) + size);
        ଯଦି (!buf) {
                r = -ENOMEM;
                ଯାଅ finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, ମୂଲ୍ୟ, size);

        r = loop_write(fd, buf, ଆକାର(uint32_t) + size);
        ଯଦି (r < 0)
                ଯାଅ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        ଯଦି (futimens(fd, /* times= */ ଖାଲି) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        ଯଦି (saved_flags_valid) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା q;

                /* Restore the original flags field, just in case */
                ଯଦି (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                ଅନ୍ୟଥା
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                ଯଦି (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା efi_set_variable_string(ଅଚଳ ଅକ୍ଷର *variable, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        _cleanup_free_ char16_t *u16 = ଖାଲି;

        u16 = utf8_to_utf16(ମୂଲ୍ୟ, SIZE_MAX);
        ଯଦି (!u16)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * ଆକାର(char16_t));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        ଫେରନ୍ତୁ (cache_efi_boot = b);
}

bool is_efi_boot(ଶୂନ୍ୟ) {
        ଯଦି (cache_efi_boot >= 0)
                ଫେରନ୍ତୁ cache_efi_boot;

        ଯଦି (detect_container() > 0)
                ଫେରନ୍ତୁ (cache_efi_boot = false);

        ଯଦି (access("/sys/firmware/efi/", F_OK) < 0) {
                ଯଦି (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ଫେରନ୍ତୁ (cache_efi_boot = false);
        }

        ଫେରନ୍ତୁ (cache_efi_boot = true);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_flag(ଅଚଳ ଅକ୍ଷର *variable) {
        _cleanup_free_ ଶୂନ୍ୟ *v = ଖାଲି;
        uint8_t b;
        size_t s;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ଫେରନ୍ତୁ 0;

        r = efi_get_variable(variable, ଖାଲି, &v, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (s != 1)
                ଫେରନ୍ତୁ -EINVAL;

        b = *(uint8_t *)v;
        ଫେରନ୍ତୁ !!b;
}

bool is_efi_secure_boot(ଶୂନ୍ୟ) {
        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache = -1;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                ଯଦି (r == -ENOENT)
                        cache = false;
                ଅନ୍ୟଥା ଯଦି (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                ଅନ୍ୟଥା
                        cache = r;
        }

        ଫେରନ୍ତୁ cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(ଶୂନ୍ୟ) {
        ସ୍ଥିର SecureBootMode cache = _SECURE_BOOT_INVALID;

        ଯଦି (cache != _SECURE_BOOT_INVALID)
                ଫେରନ୍ତୁ cache;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        ଯଦି (secure < 0) {
                ଯଦି (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ଫେରନ୍ତୁ (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        ଫେରନ୍ତୁ (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

ଅକ୍ଷର *efi_tilt_backslashes(ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ string_replace_char(s, '\\', '/');
}
