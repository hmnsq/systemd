/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <stdlib.h>
#समावेश <sys/stat.h>
#समावेश <sys/uio.h>
#समावेश <time.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "chattr-util.h"
#समावेश "efivars.h"
#समावेश "fd-util.h"
#समावेश "io-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "time-util.h"
#समावेश "utf8.h"
#समावेश "virt.h"

#अगर ENABLE_EFI

/* Reads from efivarfs sometimes fail with EINTR. Retry that many times. */
#घोषणा EFI_N_RETRIES_NO_DELAY 20
#घोषणा EFI_N_RETRIES_योग 25
#घोषणा EFI_RETRY_DELAY (50 * USEC_PER_MSEC)

पूर्णांक efi_get_variable(
                स्थिर अक्षर *variable,
                uint32_t *ret_attribute,
                व्योम **ret_मूल्य,
                माप_प्रकार *ret_आकार) {

        पूर्णांक r;
        usec_t begin = 0; /* Unnecessary initialization to appease gcc */

        निश्चित(variable);

        स्थिर अक्षर *p = strjoina("/sys/firmware/efi/efivars/", variable);

        अगर (DEBUG_LOGGING) {
                log_debug("Reading EFI variable %s.", p);
                begin = now(CLOCK_MONOTONIC);
        }

        _cleanup_close_ पूर्णांक fd = खोलो(p, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        अगर (fd < 0)
                वापस log_debug_errno(त्रुटि_सं, "open(\"%s\") failed: %m", p);

        uint32_t attr;
        _cleanup_free_ अक्षर *buf = शून्य;
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
        क्रम (अचिन्हित try = 0;; try++) {
                काष्ठा stat st;

                अगर (ख_स्थिति(fd, &st) < 0)
                        वापस log_debug_errno(त्रुटि_सं, "fstat(\"%s\") failed: %m", p);

                r = stat_verify_regular(&st);
                अगर (r < 0)
                        वापस log_debug_errno(r, "EFI variable '%s' is not a regular file, refusing: %m", p);

                अगर (st.st_size == 0) /* for uncommitted variables, see below */
                        वापस log_debug_errno(SYNTHETIC_ERRNO(ENOENT), "EFI variable '%s' is uncommitted", p);
                अगर ((uint64_t) st.st_size < माप(attr))
                        वापस log_debug_errno(SYNTHETIC_ERRNO(ENODATA), "EFI variable '%s' is shorter than %zu bytes, refusing.", p, माप(attr));
                अगर ((uint64_t) st.st_size > माप(attr) + 4 * U64_MB)
                        वापस log_debug_errno(SYNTHETIC_ERRNO(E2BIG), "EFI variable '%s' is ridiculously large, refusing.", p);

                अगर (!ret_attribute && !ret_मूल्य) {
                        /* No need to read anything, return the reported size. */
                        n = st.st_size;
                        अवरोध;
                }

                /* We want +1 for the read call, and +3 for the additional terminating bytes added below. */
                माप_प्रकार file_आकार = (माप_प्रकार) st.st_size, payload_आकार, read_आकार, alloc_आकार, read_limit;
                अगर (!SUB_SAFE(&payload_आकार, file_आकार, माप(attr)) ||
                    !ADD_SAFE(&read_आकार, payload_आकार, 1) ||
                    !ADD_SAFE(&alloc_आकार, read_आकार, 3) ||
                    !ADD_SAFE(&read_limit, file_आकार, 1))
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EOVERFLOW), "EFI variable '%s' size calculation overflow, refusing.", p);

                मुक्त(buf);
                buf = दो_स्मृति(alloc_आकार);
                अगर (!buf)
                        वापस -ENOMEM;

                काष्ठा iovec iov[] = {
                        { &attr, माप(attr)                           },
                        { buf,   read_आकार                              },
                };

                n = readv(fd, iov, 2);
                अगर (n < 0) {
                        अगर (त्रुटि_सं != EINTR)
                                वापस log_debug_errno(त्रुटि_सं, "Reading from '%s' failed: %m", p);

                        log_debug("Reading from '%s' failed with EINTR, retrying.", p);
                } अन्यथा अगर ((माप_प्रकार) n == read_limit)
                        /* We need to try again with a bigger buffer, the variable was apparently changed concurrently? */
                        log_debug("EFI variable '%s' larger than expected, retrying.", p);
                अन्यथा {
                        निश्चित((माप_प्रकार) n < read_limit);
                        अवरोध;
                }

                अगर (try >= EFI_N_RETRIES_योग)
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EBUSY), "Reading EFI variable '%s' failed even after %u tries, giving up.", p, try);
                अगर (try >= EFI_N_RETRIES_NO_DELAY)
                        (व्योम) usleep_safe(EFI_RETRY_DELAY);

                /* Start from the beginning */
                (व्योम) lseek(fd, 0, शुरू_से);
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
        अगर (n == 0)
                वापस log_debug_errno(SYNTHETIC_ERRNO(ENOENT),
                                       "EFI variable %s is uncommitted", p);
        अगर ((माप_प्रकार) n < माप(attr))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Read %zi bytes from EFI variable %s, expected >= %zu", n, p, माप(attr));
        माप_प्रकार मूल्य_आकार = n - माप(attr);

        अगर (ret_attribute)
                *ret_attribute = attr;

        अगर (ret_मूल्य) {
                निश्चित(buf);
                /* Always NUL-terminate (3 bytes, to properly protect UTF-16, even if truncated in
                 * the middle of a character) */
                buf[मूल्य_आकार] = 0;
                buf[मूल्य_आकार + 1] = 0;
                buf[मूल्य_आकार + 2] = 0;
                *ret_मूल्य = TAKE_PTR(buf);
        }

        अगर (DEBUG_LOGGING) {
                usec_t end = now(CLOCK_MONOTONIC);
                अगर (end > begin + EFI_RETRY_DELAY)
                        log_debug("Detected slow EFI variable read access on %s: %s",
                                  variable, FORMAT_TIMESPAN(end - begin, 1));
        }

        /* Note that efivarfs interestingly doesn't require ftruncate() to update an existing EFI variable
         * with a smaller value. */

        अगर (ret_आकार)
                *ret_आकार = मूल्य_आकार;

        वापस 0;
}

पूर्णांक efi_get_variable_string(स्थिर अक्षर *variable, अक्षर **ret) {
        _cleanup_free_ व्योम *s = शून्य, *x = शून्य;
        माप_प्रकार ss = 0;
        पूर्णांक r;

        निश्चित(variable);

        r = efi_get_variable(variable, शून्य, &s, &ss);
        अगर (r < 0)
                वापस r;

        x = utf16_to_utf8(s, ss);
        अगर (!x)
                वापस -ENOMEM;

        अगर (ret)
                *ret = TAKE_PTR(x);

        वापस 0;
}

पूर्णांक efi_get_variable_path(स्थिर अक्षर *variable, अक्षर **ret) {
        पूर्णांक r;

        निश्चित(variable);

        r = efi_get_variable_string(variable, ret);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                efi_tilt_backslashes(*ret);

        वापस r;
}

अटल पूर्णांक efi_verify_variable(स्थिर अक्षर *variable, uint32_t attr, स्थिर व्योम *मूल्य, माप_प्रकार आकार) {
        _cleanup_free_ व्योम *buf = शून्य;
        माप_प्रकार n;
        uint32_t a;
        पूर्णांक r;

        निश्चित(variable);
        निश्चित(मूल्य || आकार == 0);

        r = efi_get_variable(variable, &a, &buf, &n);
        अगर (r < 0)
                वापस r;

        वापस a == attr && memcmp_nn(buf, n, मूल्य, आकार) == 0;
}

पूर्णांक efi_set_variable(स्थिर अक्षर *variable, स्थिर व्योम *मूल्य, माप_प्रकार आकार) {
        अटल स्थिर uint32_t attr = EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS;

        _cleanup_free_ काष्ठा var {
                uint32_t attr;
                अक्षर buf[];
        } _packed_ *buf = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        bool saved_flags_valid = false;
        अचिन्हित saved_flags;
        पूर्णांक r;

        निश्चित(variable);
        निश्चित(मूल्य || आकार == 0);

        /* size 0 means removal, empty variable would not be enough for that */
        अगर (आकार > 0 && efi_verify_variable(variable, attr, मूल्य, आकार) > 0) {
                log_debug("Variable '%s' is already in wanted state, skipping write.", variable);
                वापस 0;
        }

        स्थिर अक्षर *p = strjoina("/sys/firmware/efi/efivars/", variable);

        /* Newer efivarfs protects variables that are not in an allow list with FS_IMMUTABLE_FL by default,
         * to protect them for accidental removal and modification. We are not changing these variables
         * accidentally however, hence let's unset the bit first. */

        r = chattr_full(AT_FDCWD, p,
                        /* value= */ 0,
                        /* mask= */ FS_IMMUTABLE_FL,
                        /* ret_previous= */ &saved_flags,
                        /* ret_final= */ शून्य,
                        /* flags= */ 0);
        अगर (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to drop FS_IMMUTABLE_FL flag from '%s', ignoring: %m", p);

        saved_flags_valid = r >= 0;

        अगर (आकार == 0) {
                अगर (unlink(p) < 0) {
                        r = -त्रुटि_सं;
                        जाओ finish;
                }

                वापस 0;
        }

        fd = खोलो(p, O_WRONLY|O_CREAT|O_NOCTTY|O_CLOEXEC, 0644);
        अगर (fd < 0) {
                r = -त्रुटि_सं;
                जाओ finish;
        }

        buf = दो_स्मृति(माप(uint32_t) + आकार);
        अगर (!buf) {
                r = -ENOMEM;
                जाओ finish;
        }

        buf->attr = attr;
        स_नकल(buf->buf, मूल्य, आकार);

        r = loop_write(fd, buf, माप(uint32_t) + आकार);
        अगर (r < 0)
                जाओ finish;

        /* For some reason efivarfs doesn't update mtime automatically. Let's do it manually then. This is
         * useful for processes that cache EFI variables to detect when changes occurred. */
        अगर (futimens(fd, /* times= */ शून्य) < 0)
                log_debug_errno(त्रुटि_सं, "Failed to update mtime/atime on %s, ignoring: %m", p);

        r = 0;

finish:
        अगर (saved_flags_valid) {
                पूर्णांक q;

                /* Restore the original flags field, just in case */
                अगर (fd < 0)
                        q = chattr_path(p, saved_flags, FS_IMMUTABLE_FL);
                अन्यथा
                        q = chattr_fd(fd, saved_flags, FS_IMMUTABLE_FL);
                अगर (q < 0)
                        log_debug_errno(q, "Failed to restore FS_IMMUTABLE_FL on '%s', ignoring: %m", p);
        }

        वापस r;
}

पूर्णांक efi_set_variable_string(स्थिर अक्षर *variable, स्थिर अक्षर *मूल्य) {
        _cleanup_free_ char16_t *u16 = शून्य;

        u16 = utf8_to_utf16(मूल्य, SIZE_MAX);
        अगर (!u16)
                वापस -ENOMEM;

        वापस efi_set_variable(variable, u16, (char16_strlen(u16) + 1) * माप(char16_t));
}

अटल पूर्णांक cache_efi_boot = -1;

bool set_efi_boot(bool b) {
        वापस (cache_efi_boot = b);
}

bool is_efi_boot(व्योम) {
        अगर (cache_efi_boot >= 0)
                वापस cache_efi_boot;

        अगर (detect_container() > 0)
                वापस (cache_efi_boot = false);

        अगर (access("/sys/firmware/efi/", F_OK) < 0) {
                अगर (त्रुटि_सं != ENOENT)
                        log_debug_errno(त्रुटि_सं, "Unable to test whether /sys/firmware/efi/ exists, assuming EFI not available: %m");
                वापस (cache_efi_boot = false);
        }

        वापस (cache_efi_boot = true);
}

अटल पूर्णांक read_flag(स्थिर अक्षर *variable) {
        _cleanup_free_ व्योम *v = शून्य;
        uint8_t b;
        माप_प्रकार s;
        पूर्णांक r;

        अगर (!is_efi_boot()) /* If this is not an EFI boot, assume the queried flags are zero */
                वापस 0;

        r = efi_get_variable(variable, शून्य, &v, &s);
        अगर (r < 0)
                वापस r;

        अगर (s != 1)
                वापस -EINVAL;

        b = *(uint8_t *)v;
        वापस !!b;
}

bool is_efi_secure_boot(व्योम) {
        अटल पूर्णांक cache = -1;
        पूर्णांक r;

        अगर (cache < 0) {
                r = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
                अगर (r == -ENOENT)
                        cache = false;
                अन्यथा अगर (r < 0)
                        log_debug_errno(r, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");
                अन्यथा
                        cache = r;
        }

        वापस cache > 0;
}

SecureBootMode efi_get_secure_boot_mode(व्योम) {
        अटल SecureBootMode cache = _SECURE_BOOT_INVALID;

        अगर (cache != _SECURE_BOOT_INVALID)
                वापस cache;

        पूर्णांक secure = read_flag(EFI_GLOBAL_VARIABLE_STR("SecureBoot"));
        अगर (secure < 0) {
                अगर (secure != -ENOENT)
                        log_debug_errno(secure, "Error reading SecureBoot EFI variable, assuming not in SecureBoot mode: %m");

                वापस (cache = SECURE_BOOT_UNSUPPORTED);
        }

        /* We can assume false for all these if they are abscent (AuditMode and
         * DeployedMode may not exist on older firmware). */
        पूर्णांक audit    = read_flag(EFI_GLOBAL_VARIABLE_STR("AuditMode"));
        पूर्णांक deployed = read_flag(EFI_GLOBAL_VARIABLE_STR("DeployedMode"));
        पूर्णांक setup    = read_flag(EFI_GLOBAL_VARIABLE_STR("SetupMode"));
        पूर्णांक moksb    = read_flag(EFI_SHIMLOCK_VARIABLE_STR("MokSBStateRT"));
        log_debug("Secure boot variables: SecureBoot=%d AuditMode=%d DeployedMode=%d SetupMode=%d MokSBStateRT=%d",
                  secure, audit, deployed, setup, moksb);

        वापस (cache = decode_secure_boot_mode(secure, audit > 0, deployed > 0, setup > 0, moksb > 0));
}
#पूर्ण_अगर

अक्षर *efi_tilt_backslashes(अक्षर *s) {
        वापस string_replace_char(s, '\\', '/');
}
