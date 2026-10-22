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

#གལ་ཏེ HAVE_PAM
        "+PAM"
#དེ་མིན
        "-PAM"
#endif

#གལ་ཏེ HAVE_AUDIT
        " +AUDIT"
#དེ་མིན
        " -AUDIT"
#endif

#གལ་ཏེ HAVE_SELINUX
        " +SELINUX"
#དེ་མིན
        " -SELINUX"
#endif

#གལ་ཏེ HAVE_APPARMOR
        " +APPARMOR"
#དེ་མིན
        " -APPARMOR"
#endif

#གལ་ཏེ ENABLE_IMA
        " +IMA"
#དེ་མིན
        " -IMA"
#endif

#གལ་ཏེ ENABLE_IPE
        " +IPE"
#དེ་མིན
        " -IPE"
#endif

#གལ་ཏེ ENABLE_SMACK
        " +SMACK"
#དེ་མིན
        " -SMACK"
#endif

#གལ་ཏེ HAVE_SECCOMP
        " +SECCOMP"
#དེ་མིན
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#གལ་ཏེ HAVE_GNUTLS
        " +GNUTLS"
#དེ་མིན
        " -GNUTLS"
#endif

#གལ་ཏེ HAVE_OPENSSL
        " +OPENSSL"
#དེ་མིན
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#གལ་ཏེ HAVE_ACL
        " +ACL"
#དེ་མིན
        " -ACL"
#endif

#གལ་ཏེ HAVE_BLKID
        " +BLKID"
#དེ་མིན
        " -BLKID"
#endif

#གལ་ཏེ HAVE_LIBCURL
        " +CURL"
#དེ་མིན
        " -CURL"
#endif

#གལ་ཏེ HAVE_ELFUTILS
        " +ELFUTILS"
#དེ་མིན
        " -ELFUTILS"
#endif

#གལ་ཏེ HAVE_LIBFIDO2
        " +FIDO2"
#དེ་མིན
        " -FIDO2"
#endif

#གལ་ཏེ HAVE_LIBIDN2
        " +IDN2"
#དེ་མིན
        " -IDN2"
#endif

#གལ་ཏེ HAVE_KMOD
        " +KMOD"
#དེ་མིན
        " -KMOD"
#endif

#གལ་ཏེ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#དེ་མིན
        " -LIBCRYPTSETUP"
#endif

#གལ་ཏེ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#དེ་མིན
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#གལ་ཏེ HAVE_LIBFDISK
        " +LIBFDISK"
#དེ་མིན
        " -LIBFDISK"
#endif

#གལ་ཏེ HAVE_PCRE2
        " +PCRE2"
#དེ་མིན
        " -PCRE2"
#endif

#གལ་ཏེ HAVE_PWQUALITY
        " +PWQUALITY"
#དེ་མིན
        " -PWQUALITY"
#endif

#གལ་ཏེ HAVE_P11KIT
        " +P11KIT"
#དེ་མིན
        " -P11KIT"
#endif

#གལ་ཏེ HAVE_QRENCODE
        " +QRENCODE"
#དེ་མིན
        " -QRENCODE"
#endif

#གལ་ཏེ HAVE_TPM2
        " +TPM2"
#དེ་མིན
        " -TPM2"
#endif

        /* compressors */

#གལ་ཏེ HAVE_BZIP2
        " +BZIP2"
#དེ་མིན
        " -BZIP2"
#endif

#གལ་ཏེ HAVE_LZ4
        " +LZ4"
#དེ་མིན
        " -LZ4"
#endif

#གལ་ཏེ HAVE_XZ
        " +XZ"
#དེ་མིན
        " -XZ"
#endif

#གལ་ཏེ HAVE_ZLIB
        " +ZLIB"
#དེ་མིན
        " -ZLIB"
#endif

#གལ་ཏེ HAVE_ZSTD
        " +ZSTD"
#དེ་མིན
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#གལ་ཏེ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#དེ་མིན
        " -BPF_FRAMEWORK"
#endif

#གལ་ཏེ HAVE_VMLINUX_H
        " +BTF"
#དེ་མིན
        " -BTF"
#endif

#གལ་ཏེ HAVE_XKBCOMMON
        " +XKBCOMMON"
#དེ་མིན
        " -XKBCOMMON"
#endif

#གལ་ཏེ ENABLE_UTMP
        " +UTMP"
#དེ་མིན
        " -UTMP"
#endif

#གལ་ཏེ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#དེ་མིན
        " -LIBARCHIVE"
#endif

#གལ་ཏེ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#དེ་མིན
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        ཧྲིལ r;

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                གལ་ཏེ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ཕྱིར NULL;
                }
                གལ་ཏེ (r == 0)
                        ཕྱིར TAKE_PTR(ret);

                གལ་ཏེ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ཕྱིར NULL;
                }

                གལ་ཏེ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                དེ་མིན གལ་ཏེ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                དེ་མིན
                        q = strextend(&ret, word);
                གལ་ཏེ (!q) {
                        log_oom_warning();
                        ཕྱིར NULL;
                }
        }
}

ཧྲིལ version(void) {
        _cleanup_free_ var_na *b = NULL;

        གལ་ཏེ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ཕྱིར 0;
}

ཧྲིལ version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ཕྱིར 0;
}

bool experimental_enabled(void) {
#གལ་ཏེ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ཕྱིར false;
#དེ་མིན
        ཧྲིལ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        གལ་ཏེ (r >= 0)
                ཕྱིར r;
        གལ་ཏེ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ཕྱིར EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
