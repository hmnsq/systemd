/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <sys/uio.h>
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chattr-util.h"
#અન્તર્ભાવ "efivars.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "utf8.h"
#અન્તર્ભાવ "virt.h"

#જો ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

પૂર્ણાંક efi_get_variable(
                અચલ અક્ષર *variable,
                uint32_t *ret_attribute,
                શૂન્ય **ret_કિંમત,
                size_t *ret_size) {

        પૂર્ણાંક r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        પ્રતિજ્ઞા(variable);

        અચલ અક્ષર *p = strjoina("/sys/firmware/efi/efivars/", variable);

        જો (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ પૂર્ણાંક fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        જો (fd < 0)
                પાછા log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ અક્ષર *buf = ખાલી;
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
        માટે (unsigned try = 0;; try++) {
                સંરચના stat st;

                જો (fstat(fd, &st) < 0)
                        પાછા log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                જો (r < 0)
                        પાછા log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                જો (st.st_size == 0) /* for uncommitted variables, see below */
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                જો ((uint64_t) st.st_size < કદ(attr))
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, કદ(attr));
                જો ((uint64_t) st.st_size > કદ(attr) + 4 * U64_MB)
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                જો (!ret_attribute && !ret_કિંમત) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        તોડો;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                જો (!SUB_SAFE(&payload_size, file_size, કદ(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                મુક્ત(buf);
                buf = malloc(alloc_size);
                જો (!buf)
                        પાછા -ENOMEM;

                સંરચના iovec iov[] = {
                        { &attr, કદ(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                જો (n < 0) {
                        જો (errno != EINTR)
                                પાછા log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } નહીંતર જો ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                નહીંતર {
                        પ્રતિજ્ઞા((size_t) n < read_limit);
                        તોડો;
                }

                જો (try >= EFI_N_RETRIES_TOTAL)
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                જો (try >= EFI_N_RETRIES_NO_DELAY)
                        (શૂન્ય) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (શૂન્ય) lseek(fd, 0, SEEK_SET);
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
        જો (n == 0)
                પાછા log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        જો ((size_t) n < કદ(attr))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, કદ(attr));
        size_t કિંમત_size = n - કદ(attr);

        જો (ret_attribute)
                *ret_attribute = attr;

        જો (ret_કિંમત) {
                પ્રતિજ્ઞા(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[કિંમત_size] = 0;
                buf[કિંમત_size + 1] = 0;
                buf[કિંમત_size + 2] = 0;
                *ret_કિંમત = TAKE_PTR(buf);
        }

        જો (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                જો (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        જો (ret_size)
                *ret_size = કિંમત_size;

        પાછા 0;
}

પૂર્ણાંક efi_get_variable_string(અચલ અક્ષર *variable, અક્ષર **ret) {
        _cleanup_free_ શૂન્ય *s = ખાલી, *x = ખાલી;
        size_t ss = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(variable);

        r = efi_get_variable(variable, ખાલી, &s, &ss);
        જો (r < 0)
                પાછા r;

        x = utf16_to_utf8(s, ss);
        જો (!x)
                પાછા -ENOMEM;

        જો (ret)
                *ret = TAKE_PTR(x);

        પાછા 0;
}

પૂર્ણાંક efi_get_variable_path(અચલ અક્ષર *variable, અક્ષર **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(variable);

        r = efi_get_variable_string(variable, ret);
        જો (r < 0)
                પાછા r;

        જો (ret)
                efi_tilt_backslashes(*ret);

        પાછા r;
}

સ્થિર પૂર્ણાંક efi_verify_variable(અચલ અક્ષર *variable, uint32_t attr, અચલ શૂન્ય *કિંમત, size_t size) {
        _cleanup_free_ શૂન્ય *buf = ખાલી;
        size_t n;
        uint32_t a;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(variable);
        પ્રતિજ્ઞા(કિંમત || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        જો (r < 0)
                પાછા r;

        પાછા a == attr && memcmp_nn(buf, n, કિંમત, size) == 0;
}

પૂર્ણાંક efi_set_variable(અચલ અક્ષર *variable, અચલ શૂન્ય *કિંમત, size_t size) {
        સ્થિર અચલ uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ સંરચના var {
                uint32_t attr;
                અક્ષર buf[];
        } _packed_ *buf = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(variable);
        પ્રતિજ્ઞા(કિંમત || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        જો (size > 0 && efi_verify_variable(variable, attr, કિંમત, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                પાછા 0;
        }

        અચલ અક્ષર *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ખાલી,
                        /* flags= */ 0);
        જો (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        જો (size == 0) {
                જો (unlink(p) < 0) {
                        r = -errno;
                        જાઓ finish;
                }

                પાછા 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        જો (fd < 0) {
                r = -errno;
                જાઓ finish;
        }

        buf = malloc(કદ(uint32_t) + size);
        જો (!buf) {
                r = -ENOMEM;
                જાઓ finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, કિંમત, size);

        r = loop_write(fd, buf, કદ(uint32_t) + size);
        જો (r < 0)
                જાઓ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        જો (futimens(fd, /* times= */ ખાલી) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        જો (saved_flags_valid) {
                પૂર્ણાંક q;

                /* Restore the original flags field, just in case */
                જો (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                નહીંતર
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                જો (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        પાછા r;
}

પૂર્ણાંક efi_set_variable_string(અચલ અક્ષર *variable, અચલ અક્ષર *કિંમત) {
        _cleanup_free_ char16_t *u16 = ખાલી;

        u16 = utf8_to_utf16(કિંમત, SIZE_MAX);
        જો (!u16)
                પાછા -ENOMEM;

        પાછા efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * કદ(char16_t));
}

સ્થિર પૂર્ણાંક cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        પાછા (cache_efi_boot = b);
}

bool is_efi_boot(શૂન્ય) {
        જો (cache_efi_boot >= 0)
                પાછા cache_efi_boot;

        જો (detect_container() > 0)
                પાછા (cache_efi_boot = false);

        જો (access("/sys/firmware/efi/", F_OK) < 0) {
                જો (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                પાછા (cache_efi_boot = false);
        }

        પાછા (cache_efi_boot = true);
}

સ્થિર પૂર્ણાંક read_flag(અચલ અક્ષર *variable) {
        _cleanup_free_ શૂન્ય *v = ખાલી;
        uint8_t b;
        size_t s;
        પૂર્ણાંક r;

        જો (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                પાછા 0;

        r = efi_get_variable(variable, ખાલી, &v, &s);
        જો (r < 0)
                પાછા r;

        જો (s != 1)
                પાછા -EINVAL;

        b = *(uint8_t *)v;
        પાછા !!b;
}

bool is_efi_secure_boot(શૂન્ય) {
        સ્થિર પૂર્ણાંક cache = -1;
        પૂર્ણાંક r;

        જો (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                જો (r == -ENOENT)
                        cache = false;
                નહીંતર જો (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                નહીંતર
                        cache = r;
        }

        પાછા cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(શૂન્ય) {
        સ્થિર SecureBootMode cache = _SECURE_BOOT_INVALID;

        જો (cache != _SECURE_BOOT_INVALID)
                પાછા cache;

        પૂર્ણાંક secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        જો (secure < 0) {
                જો (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                પાછા (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        પૂર્ણાંક audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        પૂર્ણાંક deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        પૂર્ણાંક setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        પૂર્ણાંક moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        પાછા (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

અક્ષર *efi_tilt_backslashes(અક્ષર *s) {
        પાછા string_replace_char(s, '\\', '/');
}
