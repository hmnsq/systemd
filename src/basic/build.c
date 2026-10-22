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

#もし HAVE_PAM
        "+PAM"
#否则
        "-PAM"
#endif

#もし HAVE_AUDIT
        " +AUDIT"
#否则
        " -AUDIT"
#endif

#もし HAVE_SELINUX
        " +SELINUX"
#否则
        " -SELINUX"
#endif

#もし HAVE_APPARMOR
        " +APPARMOR"
#否则
        " -APPARMOR"
#endif

#もし ENABLE_IMA
        " +IMA"
#否则
        " -IMA"
#endif

#もし ENABLE_IPE
        " +IPE"
#否则
        " -IPE"
#endif

#もし ENABLE_SMACK
        " +SMACK"
#否则
        " -SMACK"
#endif

#もし HAVE_SECCOMP
        " +SECCOMP"
#否则
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#もし HAVE_GNUTLS
        " +GNUTLS"
#否则
        " -GNUTLS"
#endif

#もし HAVE_OPENSSL
        " +OPENSSL"
#否则
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#もし HAVE_ACL
        " +ACL"
#否则
        " -ACL"
#endif

#もし HAVE_BLKID
        " +BLKID"
#否则
        " -BLKID"
#endif

#もし HAVE_LIBCURL
        " +CURL"
#否则
        " -CURL"
#endif

#もし HAVE_ELFUTILS
        " +ELFUTILS"
#否则
        " -ELFUTILS"
#endif

#もし HAVE_LIBFIDO2
        " +FIDO2"
#否则
        " -FIDO2"
#endif

#もし HAVE_LIBIDN2
        " +IDN2"
#否则
        " -IDN2"
#endif

#もし HAVE_KMOD
        " +KMOD"
#否则
        " -KMOD"
#endif

#もし HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#否则
        " -LIBCRYPTSETUP"
#endif

#もし HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#否则
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#もし HAVE_LIBFDISK
        " +LIBFDISK"
#否则
        " -LIBFDISK"
#endif

#もし HAVE_PCRE2
        " +PCRE2"
#否则
        " -PCRE2"
#endif

#もし HAVE_PWQUALITY
        " +PWQUALITY"
#否则
        " -PWQUALITY"
#endif

#もし HAVE_P11KIT
        " +P11KIT"
#否则
        " -P11KIT"
#endif

#もし HAVE_QRENCODE
        " +QRENCODE"
#否则
        " -QRENCODE"
#endif

#もし HAVE_TPM2
        " +TPM2"
#否则
        " -TPM2"
#endif

        /* compressors */

#もし HAVE_BZIP2
        " +BZIP2"
#否则
        " -BZIP2"
#endif

#もし HAVE_LZ4
        " +LZ4"
#否则
        " -LZ4"
#endif

#もし HAVE_XZ
        " +XZ"
#否则
        " -XZ"
#endif

#もし HAVE_ZLIB
        " +ZLIB"
#否则
        " -ZLIB"
#endif

#もし HAVE_ZSTD
        " +ZSTD"
#否则
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#もし BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#否则
        " -BPF_FRAMEWORK"
#endif

#もし HAVE_VMLINUX_H
        " +BTF"
#否则
        " -BTF"
#endif

#もし HAVE_XKBCOMMON
        " +XKBCOMMON"
#否则
        " -XKBCOMMON"
#endif

#もし ENABLE_UTMP
        " +UTMP"
#否则
        " -UTMP"
#endif

#もし HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#否则
        " -LIBARCHIVE"
#endif

#もし EXPERIMENTAL == EXPERIMENTAL_ENABLED
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

        ために (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                もし (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        戻る NULL;
                }
                もし (r == 0)
                        戻る TAKE_PTR(ret);

                もし (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        戻る NULL;
                }

                もし (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                否则 もし (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                否则
                        q = strextend(&ret, word);
                もし (!q) {
                        log_oom_warning();
                        戻る NULL;
                }
        }
}

整数 version(void) {
        _cleanup_free_ char *b = NULL;

        もし (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        戻る 0;
}

整数 version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        戻る 0;
}

bool experimental_enabled(void) {
#もし EXPERIMENTAL == EXPERIMENTAL_DISABLED
        戻る false;
#否则
        整数 r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        もし (r >= 0)
                戻る r;
        もし (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        戻る EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
