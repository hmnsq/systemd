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

#ᮚᮦᮔ᮪ HAVE_PAM
        "+PAM"
#ᮜᮤᮚ
        "-PAM"
#endif

#ᮚᮦᮔ᮪ HAVE_AUDIT
        " +AUDIT"
#ᮜᮤᮚ
        " -AUDIT"
#endif

#ᮚᮦᮔ᮪ HAVE_SELINUX
        " +SELINUX"
#ᮜᮤᮚ
        " -SELINUX"
#endif

#ᮚᮦᮔ᮪ HAVE_APPARMOR
        " +APPARMOR"
#ᮜᮤᮚ
        " -APPARMOR"
#endif

#ᮚᮦᮔ᮪ ENABLE_IMA
        " +IMA"
#ᮜᮤᮚ
        " -IMA"
#endif

#ᮚᮦᮔ᮪ ENABLE_IPE
        " +IPE"
#ᮜᮤᮚ
        " -IPE"
#endif

#ᮚᮦᮔ᮪ ENABLE_SMACK
        " +SMACK"
#ᮜᮤᮚ
        " -SMACK"
#endif

#ᮚᮦᮔ᮪ HAVE_SECCOMP
        " +SECCOMP"
#ᮜᮤᮚ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᮚᮦᮔ᮪ HAVE_GNUTLS
        " +GNUTLS"
#ᮜᮤᮚ
        " -GNUTLS"
#endif

#ᮚᮦᮔ᮪ HAVE_OPENSSL
        " +OPENSSL"
#ᮜᮤᮚ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᮚᮦᮔ᮪ HAVE_ACL
        " +ACL"
#ᮜᮤᮚ
        " -ACL"
#endif

#ᮚᮦᮔ᮪ HAVE_BLKID
        " +BLKID"
#ᮜᮤᮚ
        " -BLKID"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBCURL
        " +CURL"
#ᮜᮤᮚ
        " -CURL"
#endif

#ᮚᮦᮔ᮪ HAVE_ELFUTILS
        " +ELFUTILS"
#ᮜᮤᮚ
        " -ELFUTILS"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBFIDO2
        " +FIDO2"
#ᮜᮤᮚ
        " -FIDO2"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBIDN2
        " +IDN2"
#ᮜᮤᮚ
        " -IDN2"
#endif

#ᮚᮦᮔ᮪ HAVE_KMOD
        " +KMOD"
#ᮜᮤᮚ
        " -KMOD"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᮜᮤᮚ
        " -LIBCRYPTSETUP"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᮜᮤᮚ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBFDISK
        " +LIBFDISK"
#ᮜᮤᮚ
        " -LIBFDISK"
#endif

#ᮚᮦᮔ᮪ HAVE_PCRE2
        " +PCRE2"
#ᮜᮤᮚ
        " -PCRE2"
#endif

#ᮚᮦᮔ᮪ HAVE_PWQUALITY
        " +PWQUALITY"
#ᮜᮤᮚ
        " -PWQUALITY"
#endif

#ᮚᮦᮔ᮪ HAVE_P11KIT
        " +P11KIT"
#ᮜᮤᮚ
        " -P11KIT"
#endif

#ᮚᮦᮔ᮪ HAVE_QRENCODE
        " +QRENCODE"
#ᮜᮤᮚ
        " -QRENCODE"
#endif

#ᮚᮦᮔ᮪ HAVE_TPM2
        " +TPM2"
#ᮜᮤᮚ
        " -TPM2"
#endif

        /* compressors */

#ᮚᮦᮔ᮪ HAVE_BZIP2
        " +BZIP2"
#ᮜᮤᮚ
        " -BZIP2"
#endif

#ᮚᮦᮔ᮪ HAVE_LZ4
        " +LZ4"
#ᮜᮤᮚ
        " -LZ4"
#endif

#ᮚᮦᮔ᮪ HAVE_XZ
        " +XZ"
#ᮜᮤᮚ
        " -XZ"
#endif

#ᮚᮦᮔ᮪ HAVE_ZLIB
        " +ZLIB"
#ᮜᮤᮚ
        " -ZLIB"
#endif

#ᮚᮦᮔ᮪ HAVE_ZSTD
        " +ZSTD"
#ᮜᮤᮚ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᮚᮦᮔ᮪ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᮜᮤᮚ
        " -BPF_FRAMEWORK"
#endif

#ᮚᮦᮔ᮪ HAVE_VMLINUX_H
        " +BTF"
#ᮜᮤᮚ
        " -BTF"
#endif

#ᮚᮦᮔ᮪ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᮜᮤᮚ
        " -XKBCOMMON"
#endif

#ᮚᮦᮔ᮪ ENABLE_UTMP
        " +UTMP"
#ᮜᮤᮚ
        " -UTMP"
#endif

#ᮚᮦᮔ᮪ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᮜᮤᮚ
        " -LIBARCHIVE"
#endif

#ᮚᮦᮔ᮪ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᮜᮤᮚ
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
                ᮚᮦᮔ᮪ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᮝᮜᮤ NULL;
                }
                ᮚᮦᮔ᮪ (r == 0)
                        ᮝᮜᮤ TAKE_PTR(ret);

                ᮚᮦᮔ᮪ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᮝᮜᮤ NULL;
                }

                ᮚᮦᮔ᮪ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᮜᮤᮚ ᮚᮦᮔ᮪ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᮜᮤᮚ
                        q = strextend(&ret, word);
                ᮚᮦᮔ᮪ (!q) {
                        log_oom_warning();
                        ᮝᮜᮤ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᮚᮦᮔ᮪ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᮝᮜᮤ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᮝᮜᮤ 0;
}

bool experimental_enabled(void) {
#ᮚᮦᮔ᮪ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᮝᮜᮤ false;
#ᮜᮤᮚ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᮚᮦᮔ᮪ (r >= 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᮝᮜᮤ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
