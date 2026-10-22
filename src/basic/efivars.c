/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <sys/uio.h>
#அந்தர்பாவ <time.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chattr-util.h"
#அந்தர்பாவ "efivars.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "utf8.h"
#அந்தர்பாவ "virt.h"

#எனில் ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

முழு efi_get_variable(
                மாறிலி எழுத்து *variable,
                uint32_t *ret_attribute,
                வெறுமை **ret_மதிப்பு,
                size_t *ret_size) {

        முழு r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        ப்ரதிஜ்ஞா(variable);

        மாறிலி எழுத்து *p = strjoina("/sys/firmware/efi/efivars/", variable);

        எனில் (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ முழு fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ எழுத்து *buf = வெற்று;
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
        க்கு (unsigned try = 0;; try++) {
                கட்டமைப்பு stat st;

                எனில் (fstat(fd, &st) < 0)
                        திரும்பு log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                எனில் (st.st_size == 0) /* for uncommitted variables, see below */
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                எனில் ((uint64_t) st.st_size < அளவு(attr))
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, அளவு(attr));
                எனில் ((uint64_t) st.st_size > அளவு(attr) + 4 * U64_MB)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                எனில் (!ret_attribute && !ret_மதிப்பு) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        நிறுத்து;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                எனில் (!SUB_SAFE(&payload_size, file_size, அளவு(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                விடுவி(buf);
                buf = malloc(alloc_size);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                கட்டமைப்பு iovec iov[] = {
                        { &attr, அளவு(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                எனில் (n < 0) {
                        எனில் (errno != EINTR)
                                திரும்பு log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } இல்லையெனில் எனில் ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                இல்லையெனில் {
                        ப்ரதிஜ்ஞா((size_t) n < read_limit);
                        நிறுத்து;
                }

                எனில் (try >= EFI_N_RETRIES_TOTAL)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                எனில் (try >= EFI_N_RETRIES_NO_DELAY)
                        (வெறுமை) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (வெறுமை) lseek(fd, 0, SEEK_SET);
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
        எனில் (n == 0)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        எனில் ((size_t) n < அளவு(attr))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, அளவு(attr));
        size_t மதிப்பு_size = n - அளவு(attr);

        எனில் (ret_attribute)
                *ret_attribute = attr;

        எனில் (ret_மதிப்பு) {
                ப்ரதிஜ்ஞா(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[மதிப்பு_size] = 0;
                buf[மதிப்பு_size + 1] = 0;
                buf[மதிப்பு_size + 2] = 0;
                *ret_மதிப்பு = TAKE_PTR(buf);
        }

        எனில் (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                எனில் (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        எனில் (ret_size)
                *ret_size = மதிப்பு_size;

        திரும்பு 0;
}

முழு efi_get_variable_string(மாறிலி எழுத்து *variable, எழுத்து **ret) {
        _cleanup_free_ வெறுமை *s = வெற்று, *x = வெற்று;
        size_t ss = 0;
        முழு r;

        ப்ரதிஜ்ஞா(variable);

        r = efi_get_variable(variable, வெற்று, &s, &ss);
        எனில் (r < 0)
                திரும்பு r;

        x = utf16_to_utf8(s, ss);
        எனில் (!x)
                திரும்பு -ENOMEM;

        எனில் (ret)
                *ret = TAKE_PTR(x);

        திரும்பு 0;
}

முழு efi_get_variable_path(மாறிலி எழுத்து *variable, எழுத்து **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(variable);

        r = efi_get_variable_string(variable, ret);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                efi_tilt_backslashes(*ret);

        திரும்பு r;
}

நிலையான முழு efi_verify_variable(மாறிலி எழுத்து *variable, uint32_t attr, மாறிலி வெறுமை *மதிப்பு, size_t size) {
        _cleanup_free_ வெறுமை *buf = வெற்று;
        size_t n;
        uint32_t a;
        முழு r;

        ப்ரதிஜ்ஞா(variable);
        ப்ரதிஜ்ஞா(மதிப்பு || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு a == attr && memcmp_nn(buf, n, மதிப்பு, size) == 0;
}

முழு efi_set_variable(மாறிலி எழுத்து *variable, மாறிலி வெறுமை *மதிப்பு, size_t size) {
        நிலையான மாறிலி uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ கட்டமைப்பு var {
                uint32_t attr;
                எழுத்து buf[];
        } _packed_ *buf = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        முழு r;

        ப்ரதிஜ்ஞா(variable);
        ப்ரதிஜ்ஞா(மதிப்பு || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        எனில் (size > 0 && efi_verify_variable(variable, attr, மதிப்பு, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                திரும்பு 0;
        }

        மாறிலி எழுத்து *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ வெற்று,
                        /* flags= */ 0);
        எனில் (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        எனில் (size == 0) {
                எனில் (unlink(p) < 0) {
                        r = -errno;
                        செல் finish;
                }

                திரும்பு 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        எனில் (fd < 0) {
                r = -errno;
                செல் finish;
        }

        buf = malloc(அளவு(uint32_t) + size);
        எனில் (!buf) {
                r = -ENOMEM;
                செல் finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, மதிப்பு, size);

        r = loop_write(fd, buf, அளவு(uint32_t) + size);
        எனில் (r < 0)
                செல் finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        எனில் (futimens(fd, /* times= */ வெற்று) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        எனில் (saved_flags_valid) {
                முழு q;

                /* Restore the original flags field, just in case */
                எனில் (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                இல்லையெனில்
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                எனில் (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        திரும்பு r;
}

முழு efi_set_variable_string(மாறிலி எழுத்து *variable, மாறிலி எழுத்து *மதிப்பு) {
        _cleanup_free_ char16_t *u16 = வெற்று;

        u16 = utf8_to_utf16(மதிப்பு, SIZE_MAX);
        எனில் (!u16)
                திரும்பு -ENOMEM;

        திரும்பு efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * அளவு(char16_t));
}

நிலையான முழு cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        திரும்பு (cache_efi_boot = b);
}

bool is_efi_boot(வெறுமை) {
        எனில் (cache_efi_boot >= 0)
                திரும்பு cache_efi_boot;

        எனில் (detect_container() > 0)
                திரும்பு (cache_efi_boot = false);

        எனில் (access("/sys/firmware/efi/", F_OK) < 0) {
                எனில் (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                திரும்பு (cache_efi_boot = false);
        }

        திரும்பு (cache_efi_boot = true);
}

நிலையான முழு read_flag(மாறிலி எழுத்து *variable) {
        _cleanup_free_ வெறுமை *v = வெற்று;
        uint8_t b;
        size_t s;
        முழு r;

        எனில் (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                திரும்பு 0;

        r = efi_get_variable(variable, வெற்று, &v, &s);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (s != 1)
                திரும்பு -EINVAL;

        b = *(uint8_t *)v;
        திரும்பு !!b;
}

bool is_efi_secure_boot(வெறுமை) {
        நிலையான முழு cache = -1;
        முழு r;

        எனில் (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                எனில் (r == -ENOENT)
                        cache = false;
                இல்லையெனில் எனில் (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                இல்லையெனில்
                        cache = r;
        }

        திரும்பு cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(வெறுமை) {
        நிலையான SecureBootMode cache = _SECURE_BOOT_INVALID;

        எனில் (cache != _SECURE_BOOT_INVALID)
                திரும்பு cache;

        முழு secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        எனில் (secure < 0) {
                எனில் (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                திரும்பு (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        முழு audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        முழு deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        முழு setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        முழு moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        திரும்பு (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

எழுத்து *efi_tilt_backslashes(எழுத்து *s) {
        திரும்பு string_replace_char(s, '\\', '/');
}
