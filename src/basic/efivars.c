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

#如果 ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

整数 efi_get_variable(
                const char *variable,
                uint32_t *ret_attribute,
                void **ret_value,
                size_t *ret_size) {

        整数 r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        assert(variable);

        const char *p = strjoina("/sys/firmware/efi/efivars/", variable);

        如果 (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ 整数 fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        如果 (fd < 0)
                返回 log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ char *buf = NULL;
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
        为 (unsigned try = 0;; try++) {
                struct stat st;

                如果 (fstat(fd, &st) < 0)
                        返回 log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                如果 (r < 0)
                        返回 log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                如果 (st.st_size == 0) /* for uncommitted variables, see below */
                        返回 log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                如果 ((uint64_t) st.st_size < sizeof(attr))
                        返回 log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, sizeof(attr));
                如果 ((uint64_t) st.st_size > sizeof(attr) + 4 * U64_MB)
                        返回 log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                如果 (!ret_attribute && !ret_value) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        break;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                如果 (!SUB_SAFE(&payload_size, file_size, sizeof(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        返回 log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                free(buf);
                buf = malloc(alloc_size);
                如果 (!buf)
                        返回 -ENOMEM;

                struct iovec iov[] = {
                        { &attr, sizeof(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                如果 (n < 0) {
                        如果 (errno != EINTR)
                                返回 log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } 否则 如果 ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                否则 {
                        assert((size_t) n < read_limit);
                        break;
                }

                如果 (try >= EFI_N_RETRIES_TOTAL)
                        返回 log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                如果 (try >= EFI_N_RETRIES_NO_DELAY)
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
        如果 (n == 0)
                返回 log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        如果 ((size_t) n < sizeof(attr))
                返回 log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, sizeof(attr));
        size_t value_size = n - sizeof(attr);

        如果 (ret_attribute)
                *ret_attribute = attr;

        如果 (ret_value) {
                assert(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[value_size] = 0;
                buf[value_size + 1] = 0;
                buf[value_size + 2] = 0;
                *ret_value = TAKE_PTR(buf);
        }

        如果 (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                如果 (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        如果 (ret_size)
                *ret_size = value_size;

        返回 0;
}

整数 efi_get_variable_string(const char *variable, char **ret) {
        _cleanup_free_ void *s = NULL, *x = NULL;
        size_t ss = 0;
        整数 r;

        assert(variable);

        r = efi_get_variable(variable, NULL, &s, &ss);
        如果 (r < 0)
                返回 r;

        x = utf16_to_utf8(s, ss);
        如果 (!x)
                返回 -ENOMEM;

        如果 (ret)
                *ret = TAKE_PTR(x);

        返回 0;
}

整数 efi_get_variable_path(const char *variable, char **ret) {
        整数 r;

        assert(variable);

        r = efi_get_variable_string(variable, ret);
        如果 (r < 0)
                返回 r;

        如果 (ret)
                efi_tilt_backslashes(*ret);

        返回 r;
}

static 整数 efi_verify_variable(const char *variable, uint32_t attr, const void *value, size_t size) {
        _cleanup_free_ void *buf = NULL;
        size_t n;
        uint32_t a;
        整数 r;

        assert(variable);
        assert(value || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        如果 (r < 0)
                返回 r;

        返回 a == attr && memcmp_nn(buf, n, value, size) == 0;
}

整数 efi_set_variable(const char *variable, const void *value, size_t size) {
        static const uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ struct var {
                uint32_t attr;
                char buf[];
        } _packed_ *buf = NULL;
        _cleanup_close_ 整数 fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        整数 r;

        assert(variable);
        assert(value || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        如果 (size > 0 && efi_verify_variable(variable, attr, value, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                返回 0;
        }

        const char *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ NULL,
                        /* flags= */ 0);
        如果 (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        如果 (size == 0) {
                如果 (unlink(p) < 0) {
                        r = -errno;
                        goto finish;
                }

                返回 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        如果 (fd < 0) {
                r = -errno;
                goto finish;
        }

        buf = malloc(sizeof(uint32_t) + size);
        如果 (!buf) {
                r = -ENOMEM;
                goto finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, value, size);

        r = loop_write(fd, buf, sizeof(uint32_t) + size);
        如果 (r < 0)
                goto finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        如果 (futimens(fd, /* times= */ NULL) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        如果 (saved_flags_valid) {
                整数 q;

                /* Restore the original flags field, just in case */
                如果 (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                否则
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                如果 (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        返回 r;
}

整数 efi_set_variable_string(const char *variable, const char *value) {
        _cleanup_free_ char16_t *u16 = NULL;

        u16 = utf8_to_utf16(value, SIZE_MAX);
        如果 (!u16)
                返回 -ENOMEM;

        返回 efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * sizeof(char16_t));
}

static 整数 cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        返回 (cache_efi_boot = b);
}

bool is_efi_boot(void) {
        如果 (cache_efi_boot >= 0)
                返回 cache_efi_boot;

        如果 (detect_container() > 0)
                返回 (cache_efi_boot = false);

        如果 (access("/sys/firmware/efi/", F_OK) < 0) {
                如果 (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                返回 (cache_efi_boot = false);
        }

        返回 (cache_efi_boot = true);
}

static 整数 read_flag(const char *variable) {
        _cleanup_free_ void *v = NULL;
        uint8_t b;
        size_t s;
        整数 r;

        如果 (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                返回 0;

        r = efi_get_variable(variable, NULL, &v, &s);
        如果 (r < 0)
                返回 r;

        如果 (s != 1)
                返回 -EINVAL;

        b = *(uint8_t *)v;
        返回 !!b;
}

bool is_efi_secure_boot(void) {
        static 整数 cache = -1;
        整数 r;

        如果 (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                如果 (r == -ENOENT)
                        cache = false;
                否则 如果 (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                否则
                        cache = r;
        }

        返回 cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(void) {
        static SecureBootMode cache = _SECURE_BOOT_INVALID;

        如果 (cache != _SECURE_BOOT_INVALID)
                返回 cache;

        整数 secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        如果 (secure < 0) {
                如果 (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                返回 (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        整数 audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        整数 deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        整数 setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        整数 moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        返回 (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

char *efi_tilt_backslashes(char *s) {
        返回 string_replace_char(s, '\\', '/');
}
