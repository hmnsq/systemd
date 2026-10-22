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

#ᬬᬾᬦ᭄ HAVE_PAM
        "+PAM"
#ᬮᬶᬬ
        "-PAM"
#endif

#ᬬᬾᬦ᭄ HAVE_AUDIT
        " +AUDIT"
#ᬮᬶᬬ
        " -AUDIT"
#endif

#ᬬᬾᬦ᭄ HAVE_SELINUX
        " +SELINUX"
#ᬮᬶᬬ
        " -SELINUX"
#endif

#ᬬᬾᬦ᭄ HAVE_APPARMOR
        " +APPARMOR"
#ᬮᬶᬬ
        " -APPARMOR"
#endif

#ᬬᬾᬦ᭄ ENABLE_IMA
        " +IMA"
#ᬮᬶᬬ
        " -IMA"
#endif

#ᬬᬾᬦ᭄ ENABLE_IPE
        " +IPE"
#ᬮᬶᬬ
        " -IPE"
#endif

#ᬬᬾᬦ᭄ ENABLE_SMACK
        " +SMACK"
#ᬮᬶᬬ
        " -SMACK"
#endif

#ᬬᬾᬦ᭄ HAVE_SECCOMP
        " +SECCOMP"
#ᬮᬶᬬ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᬬᬾᬦ᭄ HAVE_GNUTLS
        " +GNUTLS"
#ᬮᬶᬬ
        " -GNUTLS"
#endif

#ᬬᬾᬦ᭄ HAVE_OPENSSL
        " +OPENSSL"
#ᬮᬶᬬ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᬬᬾᬦ᭄ HAVE_ACL
        " +ACL"
#ᬮᬶᬬ
        " -ACL"
#endif

#ᬬᬾᬦ᭄ HAVE_BLKID
        " +BLKID"
#ᬮᬶᬬ
        " -BLKID"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBCURL
        " +CURL"
#ᬮᬶᬬ
        " -CURL"
#endif

#ᬬᬾᬦ᭄ HAVE_ELFUTILS
        " +ELFUTILS"
#ᬮᬶᬬ
        " -ELFUTILS"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBFIDO2
        " +FIDO2"
#ᬮᬶᬬ
        " -FIDO2"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBIDN2
        " +IDN2"
#ᬮᬶᬬ
        " -IDN2"
#endif

#ᬬᬾᬦ᭄ HAVE_KMOD
        " +KMOD"
#ᬮᬶᬬ
        " -KMOD"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᬮᬶᬬ
        " -LIBCRYPTSETUP"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᬮᬶᬬ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBFDISK
        " +LIBFDISK"
#ᬮᬶᬬ
        " -LIBFDISK"
#endif

#ᬬᬾᬦ᭄ HAVE_PCRE2
        " +PCRE2"
#ᬮᬶᬬ
        " -PCRE2"
#endif

#ᬬᬾᬦ᭄ HAVE_PWQUALITY
        " +PWQUALITY"
#ᬮᬶᬬ
        " -PWQUALITY"
#endif

#ᬬᬾᬦ᭄ HAVE_P11KIT
        " +P11KIT"
#ᬮᬶᬬ
        " -P11KIT"
#endif

#ᬬᬾᬦ᭄ HAVE_QRENCODE
        " +QRENCODE"
#ᬮᬶᬬ
        " -QRENCODE"
#endif

#ᬬᬾᬦ᭄ HAVE_TPM2
        " +TPM2"
#ᬮᬶᬬ
        " -TPM2"
#endif

        /* compressors */

#ᬬᬾᬦ᭄ HAVE_BZIP2
        " +BZIP2"
#ᬮᬶᬬ
        " -BZIP2"
#endif

#ᬬᬾᬦ᭄ HAVE_LZ4
        " +LZ4"
#ᬮᬶᬬ
        " -LZ4"
#endif

#ᬬᬾᬦ᭄ HAVE_XZ
        " +XZ"
#ᬮᬶᬬ
        " -XZ"
#endif

#ᬬᬾᬦ᭄ HAVE_ZLIB
        " +ZLIB"
#ᬮᬶᬬ
        " -ZLIB"
#endif

#ᬬᬾᬦ᭄ HAVE_ZSTD
        " +ZSTD"
#ᬮᬶᬬ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᬬᬾᬦ᭄ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᬮᬶᬬ
        " -BPF_FRAMEWORK"
#endif

#ᬬᬾᬦ᭄ HAVE_VMLINUX_H
        " +BTF"
#ᬮᬶᬬ
        " -BTF"
#endif

#ᬬᬾᬦ᭄ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᬮᬶᬬ
        " -XKBCOMMON"
#endif

#ᬬᬾᬦ᭄ ENABLE_UTMP
        " +UTMP"
#ᬮᬶᬬ
        " -UTMP"
#endif

#ᬬᬾᬦ᭄ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᬮᬶᬬ
        " -LIBARCHIVE"
#endif

#ᬬᬾᬦ᭄ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᬮᬶᬬ
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
                ᬬᬾᬦ᭄ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᬯᬮᬶ NULL;
                }
                ᬬᬾᬦ᭄ (r == 0)
                        ᬯᬮᬶ TAKE_PTR(ret);

                ᬬᬾᬦ᭄ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᬯᬮᬶ NULL;
                }

                ᬬᬾᬦ᭄ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᬮᬶᬬ ᬬᬾᬦ᭄ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᬮᬶᬬ
                        q = strextend(&ret, word);
                ᬬᬾᬦ᭄ (!q) {
                        log_oom_warning();
                        ᬯᬮᬶ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᬬᬾᬦ᭄ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᬯᬮᬶ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᬯᬮᬶ 0;
}

bool experimental_enabled(void) {
#ᬬᬾᬦ᭄ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᬯᬮᬶ false;
#ᬮᬶᬬ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᬬᬾᬦ᭄ (r >= 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᬯᬮᬶ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
