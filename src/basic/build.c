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

#ᤀᤡ᤺ HAVE_PAM
        "+PAM"
#ᤀᤡ᤺ᤆᤡ
        "-PAM"
#endif

#ᤀᤡ᤺ HAVE_AUDIT
        " +AUDIT"
#ᤀᤡ᤺ᤆᤡ
        " -AUDIT"
#endif

#ᤀᤡ᤺ HAVE_SELINUX
        " +SELINUX"
#ᤀᤡ᤺ᤆᤡ
        " -SELINUX"
#endif

#ᤀᤡ᤺ HAVE_APPARMOR
        " +APPARMOR"
#ᤀᤡ᤺ᤆᤡ
        " -APPARMOR"
#endif

#ᤀᤡ᤺ ENABLE_IMA
        " +IMA"
#ᤀᤡ᤺ᤆᤡ
        " -IMA"
#endif

#ᤀᤡ᤺ ENABLE_IPE
        " +IPE"
#ᤀᤡ᤺ᤆᤡ
        " -IPE"
#endif

#ᤀᤡ᤺ ENABLE_SMACK
        " +SMACK"
#ᤀᤡ᤺ᤆᤡ
        " -SMACK"
#endif

#ᤀᤡ᤺ HAVE_SECCOMP
        " +SECCOMP"
#ᤀᤡ᤺ᤆᤡ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᤀᤡ᤺ HAVE_GNUTLS
        " +GNUTLS"
#ᤀᤡ᤺ᤆᤡ
        " -GNUTLS"
#endif

#ᤀᤡ᤺ HAVE_OPENSSL
        " +OPENSSL"
#ᤀᤡ᤺ᤆᤡ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᤀᤡ᤺ HAVE_ACL
        " +ACL"
#ᤀᤡ᤺ᤆᤡ
        " -ACL"
#endif

#ᤀᤡ᤺ HAVE_BLKID
        " +BLKID"
#ᤀᤡ᤺ᤆᤡ
        " -BLKID"
#endif

#ᤀᤡ᤺ HAVE_LIBCURL
        " +CURL"
#ᤀᤡ᤺ᤆᤡ
        " -CURL"
#endif

#ᤀᤡ᤺ HAVE_ELFUTILS
        " +ELFUTILS"
#ᤀᤡ᤺ᤆᤡ
        " -ELFUTILS"
#endif

#ᤀᤡ᤺ HAVE_LIBFIDO2
        " +FIDO2"
#ᤀᤡ᤺ᤆᤡ
        " -FIDO2"
#endif

#ᤀᤡ᤺ HAVE_LIBIDN2
        " +IDN2"
#ᤀᤡ᤺ᤆᤡ
        " -IDN2"
#endif

#ᤀᤡ᤺ HAVE_KMOD
        " +KMOD"
#ᤀᤡ᤺ᤆᤡ
        " -KMOD"
#endif

#ᤀᤡ᤺ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᤀᤡ᤺ᤆᤡ
        " -LIBCRYPTSETUP"
#endif

#ᤀᤡ᤺ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᤀᤡ᤺ᤆᤡ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᤀᤡ᤺ HAVE_LIBFDISK
        " +LIBFDISK"
#ᤀᤡ᤺ᤆᤡ
        " -LIBFDISK"
#endif

#ᤀᤡ᤺ HAVE_PCRE2
        " +PCRE2"
#ᤀᤡ᤺ᤆᤡ
        " -PCRE2"
#endif

#ᤀᤡ᤺ HAVE_PWQUALITY
        " +PWQUALITY"
#ᤀᤡ᤺ᤆᤡ
        " -PWQUALITY"
#endif

#ᤀᤡ᤺ HAVE_P11KIT
        " +P11KIT"
#ᤀᤡ᤺ᤆᤡ
        " -P11KIT"
#endif

#ᤀᤡ᤺ HAVE_QRENCODE
        " +QRENCODE"
#ᤀᤡ᤺ᤆᤡ
        " -QRENCODE"
#endif

#ᤀᤡ᤺ HAVE_TPM2
        " +TPM2"
#ᤀᤡ᤺ᤆᤡ
        " -TPM2"
#endif

        /* compressors */

#ᤀᤡ᤺ HAVE_BZIP2
        " +BZIP2"
#ᤀᤡ᤺ᤆᤡ
        " -BZIP2"
#endif

#ᤀᤡ᤺ HAVE_LZ4
        " +LZ4"
#ᤀᤡ᤺ᤆᤡ
        " -LZ4"
#endif

#ᤀᤡ᤺ HAVE_XZ
        " +XZ"
#ᤀᤡ᤺ᤆᤡ
        " -XZ"
#endif

#ᤀᤡ᤺ HAVE_ZLIB
        " +ZLIB"
#ᤀᤡ᤺ᤆᤡ
        " -ZLIB"
#endif

#ᤀᤡ᤺ HAVE_ZSTD
        " +ZSTD"
#ᤀᤡ᤺ᤆᤡ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᤀᤡ᤺ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᤀᤡ᤺ᤆᤡ
        " -BPF_FRAMEWORK"
#endif

#ᤀᤡ᤺ HAVE_VMLINUX_H
        " +BTF"
#ᤀᤡ᤺ᤆᤡ
        " -BTF"
#endif

#ᤀᤡ᤺ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᤀᤡ᤺ᤆᤡ
        " -XKBCOMMON"
#endif

#ᤀᤡ᤺ ENABLE_UTMP
        " +UTMP"
#ᤀᤡ᤺ᤆᤡ
        " -UTMP"
#endif

#ᤀᤡ᤺ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᤀᤡ᤺ᤆᤡ
        " -LIBARCHIVE"
#endif

#ᤀᤡ᤺ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᤀᤡ᤺ᤆᤡ
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
                ᤀᤡ᤺ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᤀᤡᤴ NULL;
                }
                ᤀᤡ᤺ (r == 0)
                        ᤀᤡᤴ TAKE_PTR(ret);

                ᤀᤡ᤺ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᤀᤡᤴ NULL;
                }

                ᤀᤡ᤺ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᤀᤡ᤺ᤆᤡ
                        q = strextend(&ret, word);
                ᤀᤡ᤺ (!q) {
                        log_oom_warning();
                        ᤀᤡᤴ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᤀᤡ᤺ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᤀᤡᤴ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᤀᤡᤴ 0;
}

bool experimental_enabled(void) {
#ᤀᤡ᤺ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᤀᤡᤴ false;
#ᤀᤡ᤺ᤆᤡ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᤀᤡ᤺ (r >= 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᤀᤡᤴ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
