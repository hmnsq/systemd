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

#ᰀᰧ HAVE_PAM
        "+PAM"
#ᰀᰧᰀ
        "-PAM"
#endif

#ᰀᰧ HAVE_AUDIT
        " +AUDIT"
#ᰀᰧᰀ
        " -AUDIT"
#endif

#ᰀᰧ HAVE_SELINUX
        " +SELINUX"
#ᰀᰧᰀ
        " -SELINUX"
#endif

#ᰀᰧ HAVE_APPARMOR
        " +APPARMOR"
#ᰀᰧᰀ
        " -APPARMOR"
#endif

#ᰀᰧ ENABLE_IMA
        " +IMA"
#ᰀᰧᰀ
        " -IMA"
#endif

#ᰀᰧ ENABLE_IPE
        " +IPE"
#ᰀᰧᰀ
        " -IPE"
#endif

#ᰀᰧ ENABLE_SMACK
        " +SMACK"
#ᰀᰧᰀ
        " -SMACK"
#endif

#ᰀᰧ HAVE_SECCOMP
        " +SECCOMP"
#ᰀᰧᰀ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ᰀᰧ HAVE_GNUTLS
        " +GNUTLS"
#ᰀᰧᰀ
        " -GNUTLS"
#endif

#ᰀᰧ HAVE_OPENSSL
        " +OPENSSL"
#ᰀᰧᰀ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ᰀᰧ HAVE_ACL
        " +ACL"
#ᰀᰧᰀ
        " -ACL"
#endif

#ᰀᰧ HAVE_BLKID
        " +BLKID"
#ᰀᰧᰀ
        " -BLKID"
#endif

#ᰀᰧ HAVE_LIBCURL
        " +CURL"
#ᰀᰧᰀ
        " -CURL"
#endif

#ᰀᰧ HAVE_ELFUTILS
        " +ELFUTILS"
#ᰀᰧᰀ
        " -ELFUTILS"
#endif

#ᰀᰧ HAVE_LIBFIDO2
        " +FIDO2"
#ᰀᰧᰀ
        " -FIDO2"
#endif

#ᰀᰧ HAVE_LIBIDN2
        " +IDN2"
#ᰀᰧᰀ
        " -IDN2"
#endif

#ᰀᰧ HAVE_KMOD
        " +KMOD"
#ᰀᰧᰀ
        " -KMOD"
#endif

#ᰀᰧ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ᰀᰧᰀ
        " -LIBCRYPTSETUP"
#endif

#ᰀᰧ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ᰀᰧᰀ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ᰀᰧ HAVE_LIBFDISK
        " +LIBFDISK"
#ᰀᰧᰀ
        " -LIBFDISK"
#endif

#ᰀᰧ HAVE_PCRE2
        " +PCRE2"
#ᰀᰧᰀ
        " -PCRE2"
#endif

#ᰀᰧ HAVE_PWQUALITY
        " +PWQUALITY"
#ᰀᰧᰀ
        " -PWQUALITY"
#endif

#ᰀᰧ HAVE_P11KIT
        " +P11KIT"
#ᰀᰧᰀ
        " -P11KIT"
#endif

#ᰀᰧ HAVE_QRENCODE
        " +QRENCODE"
#ᰀᰧᰀ
        " -QRENCODE"
#endif

#ᰀᰧ HAVE_TPM2
        " +TPM2"
#ᰀᰧᰀ
        " -TPM2"
#endif

        /* compressors */

#ᰀᰧ HAVE_BZIP2
        " +BZIP2"
#ᰀᰧᰀ
        " -BZIP2"
#endif

#ᰀᰧ HAVE_LZ4
        " +LZ4"
#ᰀᰧᰀ
        " -LZ4"
#endif

#ᰀᰧ HAVE_XZ
        " +XZ"
#ᰀᰧᰀ
        " -XZ"
#endif

#ᰀᰧ HAVE_ZLIB
        " +ZLIB"
#ᰀᰧᰀ
        " -ZLIB"
#endif

#ᰀᰧ HAVE_ZSTD
        " +ZSTD"
#ᰀᰧᰀ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ᰀᰧ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ᰀᰧᰀ
        " -BPF_FRAMEWORK"
#endif

#ᰀᰧ HAVE_VMLINUX_H
        " +BTF"
#ᰀᰧᰀ
        " -BTF"
#endif

#ᰀᰧ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ᰀᰧᰀ
        " -XKBCOMMON"
#endif

#ᰀᰧ ENABLE_UTMP
        " +UTMP"
#ᰀᰧᰀ
        " -UTMP"
#endif

#ᰀᰧ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ᰀᰧᰀ
        " -LIBARCHIVE"
#endif

#ᰀᰧ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ᰀᰧᰀ
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
                ᰀᰧ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ᰀᰧᰌ NULL;
                }
                ᰀᰧ (r == 0)
                        ᰀᰧᰌ TAKE_PTR(ret);

                ᰀᰧ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ᰀᰧᰌ NULL;
                }

                ᰀᰧ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ᰀᰧᰀ ᰀᰧ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ᰀᰧᰀ
                        q = strextend(&ret, word);
                ᰀᰧ (!q) {
                        log_oom_warning();
                        ᰀᰧᰌ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ᰀᰧ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ᰀᰧᰌ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ᰀᰧᰌ 0;
}

bool experimental_enabled(void) {
#ᰀᰧ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ᰀᰧᰌ false;
#ᰀᰧᰀ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ᰀᰧ (r >= 0)
                ᰀᰧᰌ r;
        ᰀᰧ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ᰀᰧᰌ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
