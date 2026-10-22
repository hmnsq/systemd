/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chattr-util.h"
#include "efivars.h"
#include "fd-util.h"
#include "io-util.h"
#include "log.h"
#include "memory-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "time-util.h"
#include "utf8.h"
#include "virt.h"

#ᬬᬾᬦ᭄ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

puur_naa_nka efi_get_variable(
                const var_na *variable,
                uint32_t *ret_attribute,
                void **ret_value,
                size_t *ret_size) {

        puur_naa_nka r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        assert(variable);

        const var_na *p = strjoina("/sys/firmware/efi/efivars/", variable);

        ᬬᬾᬦ᭄ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ puur_naa_nka fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ᬬᬾᬦ᭄ (fd < 0)
                ᬯᬮᬶ log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ var_na *buf = NULL;
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
        for (unsigned try = 0;; try++) {
                sa_nracanaa stat st;

                ᬬᬾᬦ᭄ (fstat(fd, &st) < 0)
                        ᬯᬮᬶ log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                ᬬᬾᬦ᭄ (st.st_size == 0) /* for uncommitted variables, see below */
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                ᬬᬾᬦ᭄ ((uint64_t) st.st_size < parimaa_na(attr))
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, parimaa_na(attr));
                ᬬᬾᬦ᭄ ((uint64_t) st.st_size > parimaa_na(attr) + 4 * U64_MB)
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                ᬬᬾᬦ᭄ (!ret_attribute && !ret_value) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        bha_nga;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                ᬬᬾᬦ᭄ (!SUB_SAFE(&payload_size, file_size, parimaa_na(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                free(buf);
                buf = malloc(alloc_size);
                ᬬᬾᬦ᭄ (!buf)
                        ᬯᬮᬶ -ENOMEM;

                sa_nracanaa iovec iov[] = {
                        { &attr, parimaa_na(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                ᬬᬾᬦ᭄ (n < 0) {
                        ᬬᬾᬦ᭄ (errno != EINTR)
                                ᬯᬮᬶ log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } ᬮᬶᬬ ᬬᬾᬦ᭄ ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                ᬮᬶᬬ {
                        assert((size_t) n < read_limit);
                        bha_nga;
                }

                ᬬᬾᬦ᭄ (try >= EFI_N_RETRIES_TOTAL)
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                ᬬᬾᬦ᭄ (try >= EFI_N_RETRIES_NO_DELAY)
                        (void) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (void) lseek(fd, 0, SEEK_SET);
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
        ᬬᬾᬦ᭄ (n == 0)
                ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        ᬬᬾᬦ᭄ ((size_t) n < parimaa_na(attr))
                ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, parimaa_na(attr));
        size_t value_size = n - parimaa_na(attr);

        ᬬᬾᬦ᭄ (ret_attribute)
                *ret_attribute = attr;

        ᬬᬾᬦ᭄ (ret_value) {
                assert(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[value_size] = 0;
                buf[value_size + 1] = 0;
                buf[value_size + 2] = 0;
                *ret_value = TAKE_PTR(buf);
        }

        ᬬᬾᬦ᭄ (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                ᬬᬾᬦ᭄ (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        ᬬᬾᬦ᭄ (ret_size)
                *ret_size = value_size;

        ᬯᬮᬶ 0;
}

puur_naa_nka efi_get_variable_string(const var_na *variable, var_na **ret) {
        _cleanup_free_ void *s = NULL, *x = NULL;
        size_t ss = 0;
        puur_naa_nka r;

        assert(variable);

        r = efi_get_variable(variable, NULL, &s, &ss);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        x = utf16_to_utf8(s, ss);
        ᬬᬾᬦ᭄ (!x)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (ret)
                *ret = TAKE_PTR(x);

        ᬯᬮᬶ 0;
}

puur_naa_nka efi_get_variable_path(const var_na *variable, var_na **ret) {
        puur_naa_nka r;

        assert(variable);

        r = efi_get_variable_string(variable, ret);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (ret)
                efi_tilt_backslashes(*ret);

        ᬯᬮᬶ r;
}

static puur_naa_nka efi_verify_variable(const var_na *variable, uint32_t attr, const void *value, size_t size) {
        _cleanup_free_ void *buf = NULL;
        size_t n;
        uint32_t a;
        puur_naa_nka r;

        assert(variable);
        assert(value || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ a == attr && memcmp_nn(buf, n, value, size) == 0;
}

puur_naa_nka efi_set_variable(const var_na *variable, const void *value, size_t size) {
        static const uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ sa_nracanaa var {
                uint32_t attr;
                var_na buf[];
        } _packed_ *buf = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        puur_naa_nka r;

        assert(variable);
        assert(value || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        ᬬᬾᬦ᭄ (size > 0 && efi_verify_variable(variable, attr, value, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                ᬯᬮᬶ 0;
        }

        const var_na *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ NULL,
                        /* flags= */ 0);
        ᬬᬾᬦ᭄ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        ᬬᬾᬦ᭄ (size == 0) {
                ᬬᬾᬦ᭄ (unlink(p) < 0) {
                        r = -errno;
                        goto finish;
                }

                ᬯᬮᬶ 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        ᬬᬾᬦ᭄ (fd < 0) {
                r = -errno;
                goto finish;
        }

        buf = malloc(parimaa_na(uint32_t) + size);
        ᬬᬾᬦ᭄ (!buf) {
                r = -ENOMEM;
                goto finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, value, size);

        r = loop_write(fd, buf, parimaa_na(uint32_t) + size);
        ᬬᬾᬦ᭄ (r < 0)
                goto finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        ᬬᬾᬦ᭄ (futimens(fd, /* times= */ NULL) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        ᬬᬾᬦ᭄ (saved_flags_valid) {
                puur_naa_nka q;

                /* Restore the original flags field, just in case */
                ᬬᬾᬦ᭄ (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                ᬮᬶᬬ
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                ᬬᬾᬦ᭄ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        ᬯᬮᬶ r;
}

puur_naa_nka efi_set_variable_string(const var_na *variable, const var_na *value) {
        _cleanup_free_ char16_t *u16 = NULL;

        u16 = utf8_to_utf16(value, SIZE_MAX);
        ᬬᬾᬦ᭄ (!u16)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * parimaa_na(char16_t));
}

static puur_naa_nka cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        ᬯᬮᬶ (cache_efi_boot = b);
}

bool is_efi_boot(void) {
        ᬬᬾᬦ᭄ (cache_efi_boot >= 0)
                ᬯᬮᬶ cache_efi_boot;

        ᬬᬾᬦ᭄ (detect_container() > 0)
                ᬯᬮᬶ (cache_efi_boot = false);

        ᬬᬾᬦ᭄ (access("/sys/firmware/efi/", F_OK) < 0) {
                ᬬᬾᬦ᭄ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                ᬯᬮᬶ (cache_efi_boot = false);
        }

        ᬯᬮᬶ (cache_efi_boot = true);
}

static puur_naa_nka read_flag(const var_na *variable) {
        _cleanup_free_ void *v = NULL;
        uint8_t b;
        size_t s;
        puur_naa_nka r;

        ᬬᬾᬦ᭄ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                ᬯᬮᬶ 0;

        r = efi_get_variable(variable, NULL, &v, &s);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (s != 1)
                ᬯᬮᬶ -EINVAL;

        b = *(uint8_t *)v;
        ᬯᬮᬶ !!b;
}

bool is_efi_secure_boot(void) {
        static puur_naa_nka cache = -1;
        puur_naa_nka r;

        ᬬᬾᬦ᭄ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                ᬬᬾᬦ᭄ (r == -ENOENT)
                        cache = false;
                ᬮᬶᬬ ᬬᬾᬦ᭄ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                ᬮᬶᬬ
                        cache = r;
        }

        ᬯᬮᬶ cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(void) {
        static SecureBootMode cache = _SECURE_BOOT_INVALID;

        ᬬᬾᬦ᭄ (cache != _SECURE_BOOT_INVALID)
                ᬯᬮᬶ cache;

        puur_naa_nka secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        ᬬᬾᬦ᭄ (secure < 0) {
                ᬬᬾᬦ᭄ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                ᬯᬮᬶ (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        puur_naa_nka audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        puur_naa_nka deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        puur_naa_nka setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        puur_naa_nka moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        ᬯᬮᬶ (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

var_na *efi_tilt_backslashes(var_na *s) {
        ᬯᬮᬶ string_replace_char(s, '\\', '/');
}
