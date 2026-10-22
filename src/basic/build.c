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

#ꦪꦼꦤ꧀ HAVE_PAM
        "+PAM"
#ꦭꦶꦪ
        "-PAM"
#endif

#ꦪꦼꦤ꧀ HAVE_AUDIT
        " +AUDIT"
#ꦭꦶꦪ
        " -AUDIT"
#endif

#ꦪꦼꦤ꧀ HAVE_SELINUX
        " +SELINUX"
#ꦭꦶꦪ
        " -SELINUX"
#endif

#ꦪꦼꦤ꧀ HAVE_APPARMOR
        " +APPARMOR"
#ꦭꦶꦪ
        " -APPARMOR"
#endif

#ꦪꦼꦤ꧀ ENABLE_IMA
        " +IMA"
#ꦭꦶꦪ
        " -IMA"
#endif

#ꦪꦼꦤ꧀ ENABLE_IPE
        " +IPE"
#ꦭꦶꦪ
        " -IPE"
#endif

#ꦪꦼꦤ꧀ ENABLE_SMACK
        " +SMACK"
#ꦭꦶꦪ
        " -SMACK"
#endif

#ꦪꦼꦤ꧀ HAVE_SECCOMP
        " +SECCOMP"
#ꦭꦶꦪ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ꦪꦼꦤ꧀ HAVE_GNUTLS
        " +GNUTLS"
#ꦭꦶꦪ
        " -GNUTLS"
#endif

#ꦪꦼꦤ꧀ HAVE_OPENSSL
        " +OPENSSL"
#ꦭꦶꦪ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ꦪꦼꦤ꧀ HAVE_ACL
        " +ACL"
#ꦭꦶꦪ
        " -ACL"
#endif

#ꦪꦼꦤ꧀ HAVE_BLKID
        " +BLKID"
#ꦭꦶꦪ
        " -BLKID"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBCURL
        " +CURL"
#ꦭꦶꦪ
        " -CURL"
#endif

#ꦪꦼꦤ꧀ HAVE_ELFUTILS
        " +ELFUTILS"
#ꦭꦶꦪ
        " -ELFUTILS"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBFIDO2
        " +FIDO2"
#ꦭꦶꦪ
        " -FIDO2"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBIDN2
        " +IDN2"
#ꦭꦶꦪ
        " -IDN2"
#endif

#ꦪꦼꦤ꧀ HAVE_KMOD
        " +KMOD"
#ꦭꦶꦪ
        " -KMOD"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ꦭꦶꦪ
        " -LIBCRYPTSETUP"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ꦭꦶꦪ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBFDISK
        " +LIBFDISK"
#ꦭꦶꦪ
        " -LIBFDISK"
#endif

#ꦪꦼꦤ꧀ HAVE_PCRE2
        " +PCRE2"
#ꦭꦶꦪ
        " -PCRE2"
#endif

#ꦪꦼꦤ꧀ HAVE_PWQUALITY
        " +PWQUALITY"
#ꦭꦶꦪ
        " -PWQUALITY"
#endif

#ꦪꦼꦤ꧀ HAVE_P11KIT
        " +P11KIT"
#ꦭꦶꦪ
        " -P11KIT"
#endif

#ꦪꦼꦤ꧀ HAVE_QRENCODE
        " +QRENCODE"
#ꦭꦶꦪ
        " -QRENCODE"
#endif

#ꦪꦼꦤ꧀ HAVE_TPM2
        " +TPM2"
#ꦭꦶꦪ
        " -TPM2"
#endif

        /* compressors */

#ꦪꦼꦤ꧀ HAVE_BZIP2
        " +BZIP2"
#ꦭꦶꦪ
        " -BZIP2"
#endif

#ꦪꦼꦤ꧀ HAVE_LZ4
        " +LZ4"
#ꦭꦶꦪ
        " -LZ4"
#endif

#ꦪꦼꦤ꧀ HAVE_XZ
        " +XZ"
#ꦭꦶꦪ
        " -XZ"
#endif

#ꦪꦼꦤ꧀ HAVE_ZLIB
        " +ZLIB"
#ꦭꦶꦪ
        " -ZLIB"
#endif

#ꦪꦼꦤ꧀ HAVE_ZSTD
        " +ZSTD"
#ꦭꦶꦪ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ꦪꦼꦤ꧀ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ꦭꦶꦪ
        " -BPF_FRAMEWORK"
#endif

#ꦪꦼꦤ꧀ HAVE_VMLINUX_H
        " +BTF"
#ꦭꦶꦪ
        " -BTF"
#endif

#ꦪꦼꦤ꧀ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ꦭꦶꦪ
        " -XKBCOMMON"
#endif

#ꦪꦼꦤ꧀ ENABLE_UTMP
        " +UTMP"
#ꦭꦶꦪ
        " -UTMP"
#endif

#ꦪꦼꦤ꧀ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ꦭꦶꦪ
        " -LIBARCHIVE"
#endif

#ꦪꦼꦤ꧀ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ꦭꦶꦪ
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        puur_naa_nka r;

        ꦏꦁꦒꦺꦴ (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ꦮꦭꦶ NULL;
                }
                ꦪꦼꦤ꧀ (r == 0)
                        ꦮꦭꦶ TAKE_PTR(ret);

                ꦪꦼꦤ꧀ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ꦮꦭꦶ NULL;
                }

                ꦪꦼꦤ꧀ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ꦭꦶꦪ ꦪꦼꦤ꧀ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ꦭꦶꦪ
                        q = strextend(&ret, word);
                ꦪꦼꦤ꧀ (!q) {
                        log_oom_warning();
                        ꦮꦭꦶ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ꦪꦼꦤ꧀ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ꦮꦭꦶ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ꦮꦭꦶ 0;
}

bool experimental_enabled(void) {
#ꦪꦼꦤ꧀ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ꦮꦭꦶ false;
#ꦭꦶꦪ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ꦪꦼꦤ꧀ (r >= 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ꦮꦭꦶ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
