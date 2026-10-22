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

#ако HAVE_PAM
        "+PAM"
#иначе
        "-PAM"
#endif

#ако HAVE_AUDIT
        " +AUDIT"
#иначе
        " -AUDIT"
#endif

#ако HAVE_SELINUX
        " +SELINUX"
#иначе
        " -SELINUX"
#endif

#ако HAVE_APPARMOR
        " +APPARMOR"
#иначе
        " -APPARMOR"
#endif

#ако ENABLE_IMA
        " +IMA"
#иначе
        " -IMA"
#endif

#ако ENABLE_IPE
        " +IPE"
#иначе
        " -IPE"
#endif

#ако ENABLE_SMACK
        " +SMACK"
#иначе
        " -SMACK"
#endif

#ако HAVE_SECCOMP
        " +SECCOMP"
#иначе
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ако HAVE_GNUTLS
        " +GNUTLS"
#иначе
        " -GNUTLS"
#endif

#ако HAVE_OPENSSL
        " +OPENSSL"
#иначе
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ако HAVE_ACL
        " +ACL"
#иначе
        " -ACL"
#endif

#ако HAVE_BLKID
        " +BLKID"
#иначе
        " -BLKID"
#endif

#ако HAVE_LIBCURL
        " +CURL"
#иначе
        " -CURL"
#endif

#ако HAVE_ELFUTILS
        " +ELFUTILS"
#иначе
        " -ELFUTILS"
#endif

#ако HAVE_LIBFIDO2
        " +FIDO2"
#иначе
        " -FIDO2"
#endif

#ако HAVE_LIBIDN2
        " +IDN2"
#иначе
        " -IDN2"
#endif

#ако HAVE_KMOD
        " +KMOD"
#иначе
        " -KMOD"
#endif

#ако HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#иначе
        " -LIBCRYPTSETUP"
#endif

#ако HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#иначе
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ако HAVE_LIBFDISK
        " +LIBFDISK"
#иначе
        " -LIBFDISK"
#endif

#ако HAVE_PCRE2
        " +PCRE2"
#иначе
        " -PCRE2"
#endif

#ако HAVE_PWQUALITY
        " +PWQUALITY"
#иначе
        " -PWQUALITY"
#endif

#ако HAVE_P11KIT
        " +P11KIT"
#иначе
        " -P11KIT"
#endif

#ако HAVE_QRENCODE
        " +QRENCODE"
#иначе
        " -QRENCODE"
#endif

#ако HAVE_TPM2
        " +TPM2"
#иначе
        " -TPM2"
#endif

        /* compressors */

#ако HAVE_BZIP2
        " +BZIP2"
#иначе
        " -BZIP2"
#endif

#ако HAVE_LZ4
        " +LZ4"
#иначе
        " -LZ4"
#endif

#ако HAVE_XZ
        " +XZ"
#иначе
        " -XZ"
#endif

#ако HAVE_ZLIB
        " +ZLIB"
#иначе
        " -ZLIB"
#endif

#ако HAVE_ZSTD
        " +ZSTD"
#иначе
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ако BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#иначе
        " -BPF_FRAMEWORK"
#endif

#ако HAVE_VMLINUX_H
        " +BTF"
#иначе
        " -BTF"
#endif

#ако HAVE_XKBCOMMON
        " +XKBCOMMON"
#иначе
        " -XKBCOMMON"
#endif

#ако ENABLE_UTMP
        " +UTMP"
#иначе
        " -UTMP"
#endif

#ако HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#иначе
        " -LIBARCHIVE"
#endif

#ако EXPERIMENTAL == EXPERIMENTAL_ENABLED
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
        цяло r;

        за (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                ако (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        върни NULL;
                }
                ако (r == 0)
                        върни TAKE_PTR(ret);

                ако (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        върни NULL;
                }

                ако (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                иначе ако (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                иначе
                        q = strextend(&ret, word);
                ако (!q) {
                        log_oom_warning();
                        върни NULL;
                }
        }
}

цяло version(void) {
        _cleanup_free_ char *b = NULL;

        ако (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        върни 0;
}

цяло version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        върни 0;
}

bool experimental_enabled(void) {
#ако EXPERIMENTAL == EXPERIMENTAL_DISABLED
        върни false;
#иначе
        цяло r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ако (r >= 0)
                върни r;
        ако (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        върни EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
