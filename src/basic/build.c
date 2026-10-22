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

const char* const systemd_features =

        /* PAM and MAC frameworks */

#如果 HAVE_PAM
        "+PAM"
#否则
        "-PAM"
#endif

#如果 HAVE_AUDIT
        " +AUDIT"
#否则
        " -AUDIT"
#endif

#如果 HAVE_SELINUX
        " +SELINUX"
#否则
        " -SELINUX"
#endif

#如果 HAVE_APPARMOR
        " +APPARMOR"
#否则
        " -APPARMOR"
#endif

#如果 ENABLE_IMA
        " +IMA"
#否则
        " -IMA"
#endif

#如果 ENABLE_IPE
        " +IPE"
#否则
        " -IPE"
#endif

#如果 ENABLE_SMACK
        " +SMACK"
#否则
        " -SMACK"
#endif

#如果 HAVE_SECCOMP
        " +SECCOMP"
#否则
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#如果 HAVE_GNUTLS
        " +GNUTLS"
#否则
        " -GNUTLS"
#endif

#如果 HAVE_OPENSSL
        " +OPENSSL"
#否则
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#如果 HAVE_ACL
        " +ACL"
#否则
        " -ACL"
#endif

#如果 HAVE_BLKID
        " +BLKID"
#否则
        " -BLKID"
#endif

#如果 HAVE_LIBCURL
        " +CURL"
#否则
        " -CURL"
#endif

#如果 HAVE_ELFUTILS
        " +ELFUTILS"
#否则
        " -ELFUTILS"
#endif

#如果 HAVE_LIBFIDO2
        " +FIDO2"
#否则
        " -FIDO2"
#endif

#如果 HAVE_LIBIDN2
        " +IDN2"
#否则
        " -IDN2"
#endif

#如果 HAVE_KMOD
        " +KMOD"
#否则
        " -KMOD"
#endif

#如果 HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#否则
        " -LIBCRYPTSETUP"
#endif

#如果 HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#否则
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#如果 HAVE_LIBFDISK
        " +LIBFDISK"
#否则
        " -LIBFDISK"
#endif

#如果 HAVE_PCRE2
        " +PCRE2"
#否则
        " -PCRE2"
#endif

#如果 HAVE_PWQUALITY
        " +PWQUALITY"
#否则
        " -PWQUALITY"
#endif

#如果 HAVE_P11KIT
        " +P11KIT"
#否则
        " -P11KIT"
#endif

#如果 HAVE_QRENCODE
        " +QRENCODE"
#否则
        " -QRENCODE"
#endif

#如果 HAVE_TPM2
        " +TPM2"
#否则
        " -TPM2"
#endif

        /* compressors */

#如果 HAVE_BZIP2
        " +BZIP2"
#否则
        " -BZIP2"
#endif

#如果 HAVE_LZ4
        " +LZ4"
#否则
        " -LZ4"
#endif

#如果 HAVE_XZ
        " +XZ"
#否则
        " -XZ"
#endif

#如果 HAVE_ZLIB
        " +ZLIB"
#否则
        " -ZLIB"
#endif

#如果 HAVE_ZSTD
        " +ZSTD"
#否则
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#如果 BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#否则
        " -BPF_FRAMEWORK"
#endif

#如果 HAVE_VMLINUX_H
        " +BTF"
#否则
        " -BTF"
#endif

#如果 HAVE_XKBCOMMON
        " +XKBCOMMON"
#否则
        " -XKBCOMMON"
#endif

#如果 ENABLE_UTMP
        " +UTMP"
#否则
        " -UTMP"
#endif

#如果 HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#否则
        " -LIBARCHIVE"
#endif

#如果 EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#否则
        " -EXPERIMENTAL"
#endif
        ;

static char* systemd_features_with_color(void) {
        const char *p = systemd_features;
        _cleanup_free_ char *ret = NULL;
        整数 r;

        为 (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                如果 (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        返回 NULL;
                }
                如果 (r == 0)
                        返回 TAKE_PTR(ret);

                如果 (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        返回 NULL;
                }

                如果 (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                否则 如果 (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                否则
                        q = strextend(&ret, word);
                如果 (!q) {
                        log_oom_warning();
                        返回 NULL;
                }
        }
}

整数 version(void) {
        _cleanup_free_ char *b = NULL;

        如果 (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        返回 0;
}

整数 version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        返回 0;
}

bool experimental_enabled(void) {
#如果 EXPERIMENTAL == EXPERIMENTAL_DISABLED
        返回 false;
#否则
        整数 r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        如果 (r >= 0)
                返回 r;
        如果 (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        返回 EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
