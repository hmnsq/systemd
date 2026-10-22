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

#ᱡᱩᱫᱤ HAVE_PAM
        "+PAM"
#ᱵᱟᱝ
        "-PAM"
#endif

#ᱡᱩᱫᱤ HAVE_AUDIT
        " +AUDIT"
#ᱵᱟᱝ
        " -AUDIT"
#endif

#ᱡᱩᱫᱤ HAVE_SELINUX
        " +SELINUX"
#ᱵᱟᱝ
        " -SELINUX"
#endif

#ᱡᱩᱫᱤ HAVE_APPARMOR
        " +APPARMOR"
#ᱵᱟᱝ
        " -APPARMOR"
#endif

#ᱡᱩᱫᱤ ENABLE_IMA
        " +IMA"
#ᱵᱟᱝ
        " -IMA"
#endif

#ᱡᱩᱫᱤ ENABLE_IPE
        " +IPE"
#ᱵᱟᱝ
        " -IPE"
#endif

#ᱡᱩᱫᱤ ENABLE_SMACK
        " +SMACK"
#ᱵᱟᱝ
        " -SMACK"
#endif

#ᱡᱩᱫᱤ HAVE_SECCOMP
        " +SECCOMP"
#ᱵᱟᱝ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᱡᱩᱫᱤ HAVE_GNUTLS
        " +GNUTLS"
#ᱵᱟᱝ
        " -GNUTLS"
#endif

#ᱡᱩᱫᱤ HAVE_OPENSSL
        " +OPENSSL"
#ᱵᱟᱝ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᱡᱩᱫᱤ HAVE_ACL
        " +ACL"
#ᱵᱟᱝ
        " -ACL"
#endif

#ᱡᱩᱫᱤ HAVE_BLKID
        " +BLKID"
#ᱵᱟᱝ
        " -BLKID"
#endif

#ᱡᱩᱫᱤ HAVE_LIBCURL
        " +CURL"
#ᱵᱟᱝ
        " -CURL"
#endif

#ᱡᱩᱫᱤ HAVE_ELFUTILS
        " +ELFUTILS"
#ᱵᱟᱝ
        " -ELFUTILS"
#endif

#ᱡᱩᱫᱤ HAVE_LIBFIDO2
        " +FIDO2"
#ᱵᱟᱝ
        " -FIDO2"
#endif

#ᱡᱩᱫᱤ HAVE_LIBIDN2
        " +IDN2"
#ᱵᱟᱝ
        " -IDN2"
#endif

#ᱡᱩᱫᱤ HAVE_KMOD
        " +KMOD"
#ᱵᱟᱝ
        " -KMOD"
#endif

#ᱡᱩᱫᱤ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᱵᱟᱝ
        " -LIBCRYPTSETUP"
#endif

#ᱡᱩᱫᱤ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᱵᱟᱝ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᱡᱩᱫᱤ HAVE_LIBFDISK
        " +LIBFDISK"
#ᱵᱟᱝ
        " -LIBFDISK"
#endif

#ᱡᱩᱫᱤ HAVE_PCRE2
        " +PCRE2"
#ᱵᱟᱝ
        " -PCRE2"
#endif

#ᱡᱩᱫᱤ HAVE_PWQUALITY
        " +PWQUALITY"
#ᱵᱟᱝ
        " -PWQUALITY"
#endif

#ᱡᱩᱫᱤ HAVE_P11KIT
        " +P11KIT"
#ᱵᱟᱝ
        " -P11KIT"
#endif

#ᱡᱩᱫᱤ HAVE_QRENCODE
        " +QRENCODE"
#ᱵᱟᱝ
        " -QRENCODE"
#endif

#ᱡᱩᱫᱤ HAVE_TPM2
        " +TPM2"
#ᱵᱟᱝ
        " -TPM2"
#endif

        /* compressors */

#ᱡᱩᱫᱤ HAVE_BZIP2
        " +BZIP2"
#ᱵᱟᱝ
        " -BZIP2"
#endif

#ᱡᱩᱫᱤ HAVE_LZ4
        " +LZ4"
#ᱵᱟᱝ
        " -LZ4"
#endif

#ᱡᱩᱫᱤ HAVE_XZ
        " +XZ"
#ᱵᱟᱝ
        " -XZ"
#endif

#ᱡᱩᱫᱤ HAVE_ZLIB
        " +ZLIB"
#ᱵᱟᱝ
        " -ZLIB"
#endif

#ᱡᱩᱫᱤ HAVE_ZSTD
        " +ZSTD"
#ᱵᱟᱝ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᱡᱩᱫᱤ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᱵᱟᱝ
        " -BPF_FRAMEWORK"
#endif

#ᱡᱩᱫᱤ HAVE_VMLINUX_H
        " +BTF"
#ᱵᱟᱝ
        " -BTF"
#endif

#ᱡᱩᱫᱤ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᱵᱟᱝ
        " -XKBCOMMON"
#endif

#ᱡᱩᱫᱤ ENABLE_UTMP
        " +UTMP"
#ᱵᱟᱝ
        " -UTMP"
#endif

#ᱡᱩᱫᱤ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᱵᱟᱝ
        " -LIBARCHIVE"
#endif

#ᱡᱩᱫᱤ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᱵᱟᱝ
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
                ᱡᱩᱫᱤ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᱨᱩᱣᱟᱹᱲ NULL;
                }
                ᱡᱩᱫᱤ (r == 0)
                        ᱨᱩᱣᱟᱹᱲ TAKE_PTR(ret);

                ᱡᱩᱫᱤ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᱨᱩᱣᱟᱹᱲ NULL;
                }

                ᱡᱩᱫᱤ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᱵᱟᱝ ᱡᱩᱫᱤ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᱵᱟᱝ
                        q = strextend(&ret, word);
                ᱡᱩᱫᱤ (!q) {
                        log_oom_warning();
                        ᱨᱩᱣᱟᱹᱲ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᱡᱩᱫᱤ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᱨᱩᱣᱟᱹᱲ 0;
}

bool experimental_enabled(void) {
#ᱡᱩᱫᱤ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᱨᱩᱣᱟᱹᱲ false;
#ᱵᱟᱝ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᱡᱩᱫᱤ (r >= 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᱨᱩᱣᱟᱹᱲ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
