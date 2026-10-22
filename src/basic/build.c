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

#ပေႃး HAVE_PAM
        "+PAM"
#ပေႃးဢမ်ႇ
        "-PAM"
#endif

#ပေႃး HAVE_AUDIT
        " +AUDIT"
#ပေႃးဢမ်ႇ
        " -AUDIT"
#endif

#ပေႃး HAVE_SELINUX
        " +SELINUX"
#ပေႃးဢမ်ႇ
        " -SELINUX"
#endif

#ပေႃး HAVE_APPARMOR
        " +APPARMOR"
#ပေႃးဢမ်ႇ
        " -APPARMOR"
#endif

#ပေႃး ENABLE_IMA
        " +IMA"
#ပေႃးဢမ်ႇ
        " -IMA"
#endif

#ပေႃး ENABLE_IPE
        " +IPE"
#ပေႃးဢမ်ႇ
        " -IPE"
#endif

#ပေႃး ENABLE_SMACK
        " +SMACK"
#ပေႃးဢမ်ႇ
        " -SMACK"
#endif

#ပေႃး HAVE_SECCOMP
        " +SECCOMP"
#ပေႃးဢမ်ႇ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ပေႃး HAVE_GNUTLS
        " +GNUTLS"
#ပေႃးဢမ်ႇ
        " -GNUTLS"
#endif

#ပေႃး HAVE_OPENSSL
        " +OPENSSL"
#ပေႃးဢမ်ႇ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ပေႃး HAVE_ACL
        " +ACL"
#ပေႃးဢမ်ႇ
        " -ACL"
#endif

#ပေႃး HAVE_BLKID
        " +BLKID"
#ပေႃးဢမ်ႇ
        " -BLKID"
#endif

#ပေႃး HAVE_LIBCURL
        " +CURL"
#ပေႃးဢမ်ႇ
        " -CURL"
#endif

#ပေႃး HAVE_ELFUTILS
        " +ELFUTILS"
#ပေႃးဢမ်ႇ
        " -ELFUTILS"
#endif

#ပေႃး HAVE_LIBFIDO2
        " +FIDO2"
#ပေႃးဢမ်ႇ
        " -FIDO2"
#endif

#ပေႃး HAVE_LIBIDN2
        " +IDN2"
#ပေႃးဢမ်ႇ
        " -IDN2"
#endif

#ပေႃး HAVE_KMOD
        " +KMOD"
#ပေႃးဢမ်ႇ
        " -KMOD"
#endif

#ပေႃး HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ပေႃးဢမ်ႇ
        " -LIBCRYPTSETUP"
#endif

#ပေႃး HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ပေႃးဢမ်ႇ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ပေႃး HAVE_LIBFDISK
        " +LIBFDISK"
#ပေႃးဢမ်ႇ
        " -LIBFDISK"
#endif

#ပေႃး HAVE_PCRE2
        " +PCRE2"
#ပေႃးဢမ်ႇ
        " -PCRE2"
#endif

#ပေႃး HAVE_PWQUALITY
        " +PWQUALITY"
#ပေႃးဢမ်ႇ
        " -PWQUALITY"
#endif

#ပေႃး HAVE_P11KIT
        " +P11KIT"
#ပေႃးဢမ်ႇ
        " -P11KIT"
#endif

#ပေႃး HAVE_QRENCODE
        " +QRENCODE"
#ပေႃးဢမ်ႇ
        " -QRENCODE"
#endif

#ပေႃး HAVE_TPM2
        " +TPM2"
#ပေႃးဢမ်ႇ
        " -TPM2"
#endif

        /* compressors */

#ပေႃး HAVE_BZIP2
        " +BZIP2"
#ပေႃးဢမ်ႇ
        " -BZIP2"
#endif

#ပေႃး HAVE_LZ4
        " +LZ4"
#ပေႃးဢမ်ႇ
        " -LZ4"
#endif

#ပေႃး HAVE_XZ
        " +XZ"
#ပေႃးဢမ်ႇ
        " -XZ"
#endif

#ပေႃး HAVE_ZLIB
        " +ZLIB"
#ပေႃးဢမ်ႇ
        " -ZLIB"
#endif

#ပေႃး HAVE_ZSTD
        " +ZSTD"
#ပေႃးဢမ်ႇ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ပေႃး BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ပေႃးဢမ်ႇ
        " -BPF_FRAMEWORK"
#endif

#ပေႃး HAVE_VMLINUX_H
        " +BTF"
#ပေႃးဢမ်ႇ
        " -BTF"
#endif

#ပေႃး HAVE_XKBCOMMON
        " +XKBCOMMON"
#ပေႃးဢမ်ႇ
        " -XKBCOMMON"
#endif

#ပေႃး ENABLE_UTMP
        " +UTMP"
#ပေႃးဢမ်ႇ
        " -UTMP"
#endif

#ပေႃး HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ပေႃးဢမ်ႇ
        " -LIBARCHIVE"
#endif

#ပေႃး EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ပေႃးဢမ်ႇ
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
                ပေႃး (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ပွၵ်ႈ NULL;
                }
                ပေႃး (r == 0)
                        ပွၵ်ႈ TAKE_PTR(ret);

                ပေႃး (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ပွၵ်ႈ NULL;
                }

                ပေႃး (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ပေႃးဢမ်ႇ ပေႃး (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ပေႃးဢမ်ႇ
                        q = strextend(&ret, word);
                ပေႃး (!q) {
                        log_oom_warning();
                        ပွၵ်ႈ NULL;
                }
        }
}

puur_naa_nka version(void) {
        _cleanup_free_ var_na *b = NULL;

        ပေႃး (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ပွၵ်ႈ 0;
}

puur_naa_nka version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ပွၵ်ႈ 0;
}

bool experimental_enabled(void) {
#ပေႃး EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ပွၵ်ႈ false;
#ပေႃးဢမ်ႇ
        puur_naa_nka r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ပေႃး (r >= 0)
                ပွၵ်ႈ r;
        ပေႃး (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ပွၵ်ႈ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
