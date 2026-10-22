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

const var_na* const systemd_features =

        /* PAM and MAC frameworks */

#ᨀᨘ HAVE_PAM
        "+PAM"
#ᨀᨘᨁ
        "-PAM"
#endif

#ᨀᨘ HAVE_AUDIT
        " +AUDIT"
#ᨀᨘᨁ
        " -AUDIT"
#endif

#ᨀᨘ HAVE_SELINUX
        " +SELINUX"
#ᨀᨘᨁ
        " -SELINUX"
#endif

#ᨀᨘ HAVE_APPARMOR
        " +APPARMOR"
#ᨀᨘᨁ
        " -APPARMOR"
#endif

#ᨀᨘ ENABLE_IMA
        " +IMA"
#ᨀᨘᨁ
        " -IMA"
#endif

#ᨀᨘ ENABLE_IPE
        " +IPE"
#ᨀᨘᨁ
        " -IPE"
#endif

#ᨀᨘ ENABLE_SMACK
        " +SMACK"
#ᨀᨘᨁ
        " -SMACK"
#endif

#ᨀᨘ HAVE_SECCOMP
        " +SECCOMP"
#ᨀᨘᨁ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᨀᨘ HAVE_GNUTLS
        " +GNUTLS"
#ᨀᨘᨁ
        " -GNUTLS"
#endif

#ᨀᨘ HAVE_OPENSSL
        " +OPENSSL"
#ᨀᨘᨁ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᨀᨘ HAVE_ACL
        " +ACL"
#ᨀᨘᨁ
        " -ACL"
#endif

#ᨀᨘ HAVE_BLKID
        " +BLKID"
#ᨀᨘᨁ
        " -BLKID"
#endif

#ᨀᨘ HAVE_LIBCURL
        " +CURL"
#ᨀᨘᨁ
        " -CURL"
#endif

#ᨀᨘ HAVE_ELFUTILS
        " +ELFUTILS"
#ᨀᨘᨁ
        " -ELFUTILS"
#endif

#ᨀᨘ HAVE_LIBFIDO2
        " +FIDO2"
#ᨀᨘᨁ
        " -FIDO2"
#endif

#ᨀᨘ HAVE_LIBIDN2
        " +IDN2"
#ᨀᨘᨁ
        " -IDN2"
#endif

#ᨀᨘ HAVE_KMOD
        " +KMOD"
#ᨀᨘᨁ
        " -KMOD"
#endif

#ᨀᨘ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᨀᨘᨁ
        " -LIBCRYPTSETUP"
#endif

#ᨀᨘ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᨀᨘᨁ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᨀᨘ HAVE_LIBFDISK
        " +LIBFDISK"
#ᨀᨘᨁ
        " -LIBFDISK"
#endif

#ᨀᨘ HAVE_PCRE2
        " +PCRE2"
#ᨀᨘᨁ
        " -PCRE2"
#endif

#ᨀᨘ HAVE_PWQUALITY
        " +PWQUALITY"
#ᨀᨘᨁ
        " -PWQUALITY"
#endif

#ᨀᨘ HAVE_P11KIT
        " +P11KIT"
#ᨀᨘᨁ
        " -P11KIT"
#endif

#ᨀᨘ HAVE_QRENCODE
        " +QRENCODE"
#ᨀᨘᨁ
        " -QRENCODE"
#endif

#ᨀᨘ HAVE_TPM2
        " +TPM2"
#ᨀᨘᨁ
        " -TPM2"
#endif

        /* compressors */

#ᨀᨘ HAVE_BZIP2
        " +BZIP2"
#ᨀᨘᨁ
        " -BZIP2"
#endif

#ᨀᨘ HAVE_LZ4
        " +LZ4"
#ᨀᨘᨁ
        " -LZ4"
#endif

#ᨀᨘ HAVE_XZ
        " +XZ"
#ᨀᨘᨁ
        " -XZ"
#endif

#ᨀᨘ HAVE_ZLIB
        " +ZLIB"
#ᨀᨘᨁ
        " -ZLIB"
#endif

#ᨀᨘ HAVE_ZSTD
        " +ZSTD"
#ᨀᨘᨁ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᨀᨘ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᨀᨘᨁ
        " -BPF_FRAMEWORK"
#endif

#ᨀᨘ HAVE_VMLINUX_H
        " +BTF"
#ᨀᨘᨁ
        " -BTF"
#endif

#ᨀᨘ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᨀᨘᨁ
        " -XKBCOMMON"
#endif

#ᨀᨘ ENABLE_UTMP
        " +UTMP"
#ᨀᨘᨁ
        " -UTMP"
#endif

#ᨀᨘ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᨀᨘᨁ
        " -LIBARCHIVE"
#endif

#ᨀᨘ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᨀᨘᨁ
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        puur_naa_nka r;

        for (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                ᨀᨘ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᨀᨘᨒ NULL;
                }
                ᨀᨘ (r == 0)
                        ᨀᨘᨒ TAKE_PTR(ret);

                ᨀᨘ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᨀᨘᨒ NULL;
                }

                ᨀᨘ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᨀᨘᨁ ᨀᨘ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᨀᨘᨁ
                        q = strextend(&ret, word);
                ᨀᨘ (!q) {
                        log_oom_warning();
                        ᨀᨘᨒ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᨀᨘ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᨀᨘᨒ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᨀᨘᨒ 0;
}

bool experimental_enabled(void) {
#ᨀᨘ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᨀᨘᨒ false;
#ᨀᨘᨁ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᨀᨘ (r >= 0)
                ᨀᨘᨒ r;
        ᨀᨘ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᨀᨘᨒ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
