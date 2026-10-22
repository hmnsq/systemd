/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <sys/uio.h>
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chattr-util.h"
#അന്തര്ഭാവ "efivars.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "utf8.h"
#അന്തര്ഭാവ "virt.h"

#എങ്കിൽ ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#define EFI_N_RETRIES_NO_DELAY 20
#define EFI_N_RETRIES_TOTAL 25
#define EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

പൂർണ്ണസംഖ്യ efi_get_variable(
                സ്ഥിരം അക്ഷരം *variable,
                uint32_t *ret_attribute,
                ശൂന്യം **ret_മൂല്യം,
                size_t *ret_size) {

        പൂർണ്ണസംഖ്യ r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        പ്രതിജ്ഞാ(variable);

        സ്ഥിരം അക്ഷരം *p = strjoina("/sys/firmware/efi/efivars/", variable);

        എങ്കിൽ (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ log_debug_errno(errno, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
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
        വേണ്ടി (unsigned try = 0;; try++) {
                ഘടന stat st;

                എങ്കിൽ (fstat(fd, &st) < 0)
                        തിരികെ log_debug_errno(errno, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                എങ്കിൽ (st.st_size == 0) /* for uncommitted variables, see below */
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                എങ്കിൽ ((uint64_t) st.st_size < വലുപ്പം(attr))
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, വലുപ്പം(attr));
                എങ്കിൽ ((uint64_t) st.st_size > വലുപ്പം(attr) + 4 * U64_MB)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                എങ്കിൽ (!ret_attribute && !ret_മൂല്യം) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        നിർത്തുക;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                size_t file_size = (size_t) st.st_size, payload_size, read_size, alloc_size, read_limit;
                എങ്കിൽ (!SUB_SAFE(&payload_size, file_size, വലുപ്പം(attr)) ||
                    !ADD_SAFE(&read_size, payload_size, 1) ||
                    !ADD_SAFE(&alloc_size, read_size, 3) ||
                    !ADD_SAFE(&read_limit, file_size, 1))
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                സ്വതന്ത്രം(buf);
                buf = malloc(alloc_size);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                ഘടന iovec iov[] = {
                        { &attr, വലുപ്പം(attr)                           },
                        { buf,   read_size                              },
                };

                n = readv(fd, iov, 2);
                എങ്കിൽ (n < 0) {
                        എങ്കിൽ (errno != EINTR)
                                തിരികെ log_debug_errno(errno, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } അല്ലെങ്കിൽ എങ്കിൽ ((size_t) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                അല്ലെങ്കിൽ {
                        പ്രതിജ്ഞാ((size_t) n < read_limit);
                        നിർത്തുക;
                }

                എങ്കിൽ (try >= EFI_N_RETRIES_TOTAL)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                എങ്കിൽ (try >= EFI_N_RETRIES_NO_DELAY)
                        (ശൂന്യം) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (ശൂന്യം) lseek(fd, 0, SEEK_SET);
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
        എങ്കിൽ (n == 0)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        എങ്കിൽ ((size_t) n < വലുപ്പം(attr))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, വലുപ്പം(attr));
        size_t മൂല്യം_size = n - വലുപ്പം(attr);

        എങ്കിൽ (ret_attribute)
                *ret_attribute = attr;

        എങ്കിൽ (ret_മൂല്യം) {
                പ്രതിജ്ഞാ(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[മൂല്യം_size] = 0;
                buf[മൂല്യം_size + 1] = 0;
                buf[മൂല്യം_size + 2] = 0;
                *ret_മൂല്യം = TAKE_PTR(buf);
        }

        എങ്കിൽ (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                എങ്കിൽ (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        എങ്കിൽ (ret_size)
                *ret_size = മൂല്യം_size;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ efi_get_variable_string(സ്ഥിരം അക്ഷരം *variable, അക്ഷരം **ret) {
        _cleanup_free_ ശൂന്യം *s = ശൂന്യ, *x = ശൂന്യ;
        size_t ss = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(variable);

        r = efi_get_variable(variable, ശൂന്യ, &s, &ss);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        x = utf16_to_utf8(s, ss);
        എങ്കിൽ (!x)
                തിരികെ -ENOMEM;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(x);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ efi_get_variable_path(സ്ഥിരം അക്ഷരം *variable, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(variable);

        r = efi_get_variable_string(variable, ret);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                efi_tilt_backslashes(*ret);

        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ efi_verify_variable(സ്ഥിരം അക്ഷരം *variable, uint32_t attr, സ്ഥിരം ശൂന്യം *മൂല്യം, size_t size) {
        _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;
        size_t n;
        uint32_t a;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(variable);
        പ്രതിജ്ഞാ(മൂല്യം || size == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ a == attr && memcmp_nn(buf, n, മൂല്യം, size) == 0;
}

പൂർണ്ണസംഖ്യ efi_set_variable(സ്ഥിരം അക്ഷരം *variable, സ്ഥിരം ശൂന്യം *മൂല്യം, size_t size) {
        സ്ഥിതം സ്ഥിരം uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ ഘടന var {
                uint32_t attr;
                അക്ഷരം buf[];
        } _packed_ *buf = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        bool saved_flags_valid = false;
        unsigned saved_flags;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(variable);
        പ്രതിജ്ഞാ(മൂല്യം || size == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        എങ്കിൽ (size > 0 && efi_verify_variable(variable, attr, മൂല്യം, size) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                തിരികെ 0;
        }

        സ്ഥിരം അക്ഷരം *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ ശൂന്യ,
                        /* flags= */ 0);
        എങ്കിൽ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        എങ്കിൽ (size == 0) {
                എങ്കിൽ (unlink(p) < 0) {
                        r = -errno;
                        പോകുക finish;
                }

                തിരികെ 0;
        }

        fd = open(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        എങ്കിൽ (fd < 0) {
                r = -errno;
                പോകുക finish;
        }

        buf = malloc(വലുപ്പം(uint32_t) + size);
        എങ്കിൽ (!buf) {
                r = -ENOMEM;
                പോകുക finish;
        }

        buf->attr = attr;
        memcpy(buf->buf, മൂല്യം, size);

        r = loop_write(fd, buf, വലുപ്പം(uint32_t) + size);
        എങ്കിൽ (r < 0)
                പോകുക finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        എങ്കിൽ (futimens(fd, /* times= */ ശൂന്യ) < 0)
                log_debug_errno(errno, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        എങ്കിൽ (saved_flags_valid) {
                പൂർണ്ണസംഖ്യ q;

                /* Restore the original flags field, just in case */
                എങ്കിൽ (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                അല്ലെങ്കിൽ
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                എങ്കിൽ (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ efi_set_variable_string(സ്ഥിരം അക്ഷരം *variable, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        _cleanup_free_ char16_t *u16 = ശൂന്യ;

        u16 = utf8_to_utf16(മൂല്യം, SIZE_MAX);
        എങ്കിൽ (!u16)
                തിരികെ -ENOMEM;

        തിരികെ efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * വലുപ്പം(char16_t));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        തിരികെ (cache_efi_boot = b);
}

bool is_efi_boot(ശൂന്യം) {
        എങ്കിൽ (cache_efi_boot >= 0)
                തിരികെ cache_efi_boot;

        എങ്കിൽ (detect_container() > 0)
                തിരികെ (cache_efi_boot = false);

        എങ്കിൽ (access("/sys/firmware/efi/", F_OK) < 0) {
                എങ്കിൽ (errno != ENOENT)
                        log_debug_errno(errno, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                തിരികെ (cache_efi_boot = false);
        }

        തിരികെ (cache_efi_boot = true);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ read_flag(സ്ഥിരം അക്ഷരം *variable) {
        _cleanup_free_ ശൂന്യം *v = ശൂന്യ;
        uint8_t b;
        size_t s;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                തിരികെ 0;

        r = efi_get_variable(variable, ശൂന്യ, &v, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (s != 1)
                തിരികെ -EINVAL;

        b = *(uint8_t *)v;
        തിരികെ !!b;
}

bool is_efi_secure_boot(ശൂന്യം) {
        സ്ഥിതം പൂർണ്ണസംഖ്യ cache = -1;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                എങ്കിൽ (r == -ENOENT)
                        cache = false;
                അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                അല്ലെങ്കിൽ
                        cache = r;
        }

        തിരികെ cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(ശൂന്യം) {
        സ്ഥിതം SecureBootMode cache = _SECURE_BOOT_INVALID;

        എങ്കിൽ (cache != _SECURE_BOOT_INVALID)
                തിരികെ cache;

        പൂർണ്ണസംഖ്യ secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        എങ്കിൽ (secure < 0) {
                എങ്കിൽ (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                തിരികെ (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        പൂർണ്ണസംഖ്യ audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        പൂർണ്ണസംഖ്യ deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        പൂർണ്ണസംഖ്യ setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        പൂർണ്ണസംഖ്യ moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        തിരികെ (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#endif

അക്ഷരം *efi_tilt_backslashes(അക്ഷരം *s) {
        തിരികെ string_replace_char(s, '\\', '/');
}
