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

#ᯀᯮᯝ HAVE_PAM
        "+PAM"
#ᯀᯮᯝᯂᯪ
        "-PAM"
#endif

#ᯀᯮᯝ HAVE_AUDIT
        " +AUDIT"
#ᯀᯮᯝᯂᯪ
        " -AUDIT"
#endif

#ᯀᯮᯝ HAVE_SELINUX
        " +SELINUX"
#ᯀᯮᯝᯂᯪ
        " -SELINUX"
#endif

#ᯀᯮᯝ HAVE_APPARMOR
        " +APPARMOR"
#ᯀᯮᯝᯂᯪ
        " -APPARMOR"
#endif

#ᯀᯮᯝ ENABLE_IMA
        " +IMA"
#ᯀᯮᯝᯂᯪ
        " -IMA"
#endif

#ᯀᯮᯝ ENABLE_IPE
        " +IPE"
#ᯀᯮᯝᯂᯪ
        " -IPE"
#endif

#ᯀᯮᯝ ENABLE_SMACK
        " +SMACK"
#ᯀᯮᯝᯂᯪ
        " -SMACK"
#endif

#ᯀᯮᯝ HAVE_SECCOMP
        " +SECCOMP"
#ᯀᯮᯝᯂᯪ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᯀᯮᯝ HAVE_GNUTLS
        " +GNUTLS"
#ᯀᯮᯝᯂᯪ
        " -GNUTLS"
#endif

#ᯀᯮᯝ HAVE_OPENSSL
        " +OPENSSL"
#ᯀᯮᯝᯂᯪ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᯀᯮᯝ HAVE_ACL
        " +ACL"
#ᯀᯮᯝᯂᯪ
        " -ACL"
#endif

#ᯀᯮᯝ HAVE_BLKID
        " +BLKID"
#ᯀᯮᯝᯂᯪ
        " -BLKID"
#endif

#ᯀᯮᯝ HAVE_LIBCURL
        " +CURL"
#ᯀᯮᯝᯂᯪ
        " -CURL"
#endif

#ᯀᯮᯝ HAVE_ELFUTILS
        " +ELFUTILS"
#ᯀᯮᯝᯂᯪ
        " -ELFUTILS"
#endif

#ᯀᯮᯝ HAVE_LIBFIDO2
        " +FIDO2"
#ᯀᯮᯝᯂᯪ
        " -FIDO2"
#endif

#ᯀᯮᯝ HAVE_LIBIDN2
        " +IDN2"
#ᯀᯮᯝᯂᯪ
        " -IDN2"
#endif

#ᯀᯮᯝ HAVE_KMOD
        " +KMOD"
#ᯀᯮᯝᯂᯪ
        " -KMOD"
#endif

#ᯀᯮᯝ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᯀᯮᯝᯂᯪ
        " -LIBCRYPTSETUP"
#endif

#ᯀᯮᯝ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᯀᯮᯝᯂᯪ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᯀᯮᯝ HAVE_LIBFDISK
        " +LIBFDISK"
#ᯀᯮᯝᯂᯪ
        " -LIBFDISK"
#endif

#ᯀᯮᯝ HAVE_PCRE2
        " +PCRE2"
#ᯀᯮᯝᯂᯪ
        " -PCRE2"
#endif

#ᯀᯮᯝ HAVE_PWQUALITY
        " +PWQUALITY"
#ᯀᯮᯝᯂᯪ
        " -PWQUALITY"
#endif

#ᯀᯮᯝ HAVE_P11KIT
        " +P11KIT"
#ᯀᯮᯝᯂᯪ
        " -P11KIT"
#endif

#ᯀᯮᯝ HAVE_QRENCODE
        " +QRENCODE"
#ᯀᯮᯝᯂᯪ
        " -QRENCODE"
#endif

#ᯀᯮᯝ HAVE_TPM2
        " +TPM2"
#ᯀᯮᯝᯂᯪ
        " -TPM2"
#endif

        /* compressors */

#ᯀᯮᯝ HAVE_BZIP2
        " +BZIP2"
#ᯀᯮᯝᯂᯪ
        " -BZIP2"
#endif

#ᯀᯮᯝ HAVE_LZ4
        " +LZ4"
#ᯀᯮᯝᯂᯪ
        " -LZ4"
#endif

#ᯀᯮᯝ HAVE_XZ
        " +XZ"
#ᯀᯮᯝᯂᯪ
        " -XZ"
#endif

#ᯀᯮᯝ HAVE_ZLIB
        " +ZLIB"
#ᯀᯮᯝᯂᯪ
        " -ZLIB"
#endif

#ᯀᯮᯝ HAVE_ZSTD
        " +ZSTD"
#ᯀᯮᯝᯂᯪ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᯀᯮᯝ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᯀᯮᯝᯂᯪ
        " -BPF_FRAMEWORK"
#endif

#ᯀᯮᯝ HAVE_VMLINUX_H
        " +BTF"
#ᯀᯮᯝᯂᯪ
        " -BTF"
#endif

#ᯀᯮᯝ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᯀᯮᯝᯂᯪ
        " -XKBCOMMON"
#endif

#ᯀᯮᯝ ENABLE_UTMP
        " +UTMP"
#ᯀᯮᯝᯂᯪ
        " -UTMP"
#endif

#ᯀᯮᯝ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᯀᯮᯝᯂᯪ
        " -LIBARCHIVE"
#endif

#ᯀᯮᯝ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᯀᯮᯝᯂᯪ
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
                ᯀᯮᯝ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᯀᯮᯞ NULL;
                }
                ᯀᯮᯝ (r == 0)
                        ᯀᯮᯞ TAKE_PTR(ret);

                ᯀᯮᯝ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᯀᯮᯞ NULL;
                }

                ᯀᯮᯝ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᯀᯮᯝᯂᯪ ᯀᯮᯝ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᯀᯮᯝᯂᯪ
                        q = strextend(&ret, word);
                ᯀᯮᯝ (!q) {
                        log_oom_warning();
                        ᯀᯮᯞ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᯀᯮᯝ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᯀᯮᯞ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᯀᯮᯞ 0;
}

bool experimental_enabled(void) {
#ᯀᯮᯝ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᯀᯮᯞ false;
#ᯀᯮᯝᯂᯪ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᯀᯮᯝ (r >= 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᯀᯮᯞ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
