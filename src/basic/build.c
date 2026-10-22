/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "ansi-color.h"
#include "build.h"
#include "env-util.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "version.h"

const char* const systemd_features =

        /* PAM and MAC frameworks */

#если HAVE_PAM
        "+PAM"
#иначе
        "-PAM"
#endif

#если HAVE_AUDIT
        " +AUDIT"
#иначе
        " -AUDIT"
#endif

#если HAVE_SELINUX
        " +SELINUX"
#иначе
        " -SELINUX"
#endif

#если HAVE_APPARMOR
        " +APPARMOR"
#иначе
        " -APPARMOR"
#endif

#если ENABLE_IMA
        " +IMA"
#иначе
        " -IMA"
#endif

#если ENABLE_IPE
        " +IPE"
#иначе
        " -IPE"
#endif

#если ENABLE_SMACK
        " +SMACK"
#иначе
        " -SMACK"
#endif

#если HAVE_SECCOMP
        " +SECCOMP"
#иначе
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#если HAVE_GNUTLS
        " +GNUTLS"
#иначе
        " -GNUTLS"
#endif

#если HAVE_OPENSSL
        " +OPENSSL"
#иначе
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#если HAVE_ACL
        " +ACL"
#иначе
        " -ACL"
#endif

#если HAVE_BLKID
        " +BLKID"
#иначе
        " -BLKID"
#endif

#если HAVE_LIBCURL
        " +CURL"
#иначе
        " -CURL"
#endif

#если HAVE_ELFUTILS
        " +ELFUTILS"
#иначе
        " -ELFUTILS"
#endif

#если HAVE_LIBFIDO2
        " +FIDO2"
#иначе
        " -FIDO2"
#endif

#если HAVE_LIBIDN2
        " +IDN2"
#иначе
        " -IDN2"
#endif

#если HAVE_KMOD
        " +KMOD"
#иначе
        " -KMOD"
#endif

#если HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#иначе
        " -LIBCRYPTSETUP"
#endif

#если HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#иначе
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#если HAVE_LIBFDISK
        " +LIBFDISK"
#иначе
        " -LIBFDISK"
#endif

#если HAVE_PCRE2
        " +PCRE2"
#иначе
        " -PCRE2"
#endif

#если HAVE_PWQUALITY
        " +PWQUALITY"
#иначе
        " -PWQUALITY"
#endif

#если HAVE_P11KIT
        " +P11KIT"
#иначе
        " -P11KIT"
#endif

#если HAVE_QRENCODE
        " +QRENCODE"
#иначе
        " -QRENCODE"
#endif

#если HAVE_TPM2
        " +TPM2"
#иначе
        " -TPM2"
#endif

        /* compressors */

#если HAVE_BZIP2
        " +BZIP2"
#иначе
        " -BZIP2"
#endif

#если HAVE_LZ4
        " +LZ4"
#иначе
        " -LZ4"
#endif

#если HAVE_XZ
        " +XZ"
#иначе
        " -XZ"
#endif

#если HAVE_ZLIB
        " +ZLIB"
#иначе
        " -ZLIB"
#endif

#если HAVE_ZSTD
        " +ZSTD"
#иначе
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#если BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#иначе
        " -BPF_FRAMEWORK"
#endif

#если HAVE_VMLINUX_H
        " +BTF"
#иначе
        " -BTF"
#endif

#если HAVE_XKBCOMMON
        " +XKBCOMMON"
#иначе
        " -XKBCOMMON"
#endif

#если ENABLE_UTMP
        " +UTMP"
#иначе
        " -UTMP"
#endif

#если HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#иначе
        " -LIBARCHIVE"
#endif

#если EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#иначе
        " -EXPERIMENTAL"
#endif
        ;

static char* systemd_features_with_color(void) {
        const char *p = systemd_features;
        _cleanup_free_ char *ret = NULL;
        целое r;

        для (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                если (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        вернуть NULL;
                }
                если (r == 0)
                        вернуть TAKE_PTR(ret);

                если (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        вернуть NULL;
                }

                если (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                иначе если (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                иначе
                        q = strextend(&ret, word);
                если (!q) {
                        log_oom_warning();
                        вернуть NULL;
                }
        }
}

целое version(void) {
        _cleanup_free_ char *b = NULL;

        если (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        вернуть 0;
}

целое version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        вернуть 0;
}

bool experimental_enabled(void) {
#если EXPERIMENTAL == EXPERIMENTAL_DISABLED
        вернуть false;
#иначе
        целое r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        если (r >= 0)
                вернуть r;
        если (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        вернуть EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
