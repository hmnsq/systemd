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

#якщо HAVE_PAM
        "+PAM"
#інакше
        "-PAM"
#endif

#якщо HAVE_AUDIT
        " +AUDIT"
#інакше
        " -AUDIT"
#endif

#якщо HAVE_SELINUX
        " +SELINUX"
#інакше
        " -SELINUX"
#endif

#якщо HAVE_APPARMOR
        " +APPARMOR"
#інакше
        " -APPARMOR"
#endif

#якщо ENABLE_IMA
        " +IMA"
#інакше
        " -IMA"
#endif

#якщо ENABLE_IPE
        " +IPE"
#інакше
        " -IPE"
#endif

#якщо ENABLE_SMACK
        " +SMACK"
#інакше
        " -SMACK"
#endif

#якщо HAVE_SECCOMP
        " +SECCOMP"
#інакше
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#якщо HAVE_GNUTLS
        " +GNUTLS"
#інакше
        " -GNUTLS"
#endif

#якщо HAVE_OPENSSL
        " +OPENSSL"
#інакше
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#якщо HAVE_ACL
        " +ACL"
#інакше
        " -ACL"
#endif

#якщо HAVE_BLKID
        " +BLKID"
#інакше
        " -BLKID"
#endif

#якщо HAVE_LIBCURL
        " +CURL"
#інакше
        " -CURL"
#endif

#якщо HAVE_ELFUTILS
        " +ELFUTILS"
#інакше
        " -ELFUTILS"
#endif

#якщо HAVE_LIBFIDO2
        " +FIDO2"
#інакше
        " -FIDO2"
#endif

#якщо HAVE_LIBIDN2
        " +IDN2"
#інакше
        " -IDN2"
#endif

#якщо HAVE_KMOD
        " +KMOD"
#інакше
        " -KMOD"
#endif

#якщо HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#інакше
        " -LIBCRYPTSETUP"
#endif

#якщо HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#інакше
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#якщо HAVE_LIBFDISK
        " +LIBFDISK"
#інакше
        " -LIBFDISK"
#endif

#якщо HAVE_PCRE2
        " +PCRE2"
#інакше
        " -PCRE2"
#endif

#якщо HAVE_PWQUALITY
        " +PWQUALITY"
#інакше
        " -PWQUALITY"
#endif

#якщо HAVE_P11KIT
        " +P11KIT"
#інакше
        " -P11KIT"
#endif

#якщо HAVE_QRENCODE
        " +QRENCODE"
#інакше
        " -QRENCODE"
#endif

#якщо HAVE_TPM2
        " +TPM2"
#інакше
        " -TPM2"
#endif

        /* compressors */

#якщо HAVE_BZIP2
        " +BZIP2"
#інакше
        " -BZIP2"
#endif

#якщо HAVE_LZ4
        " +LZ4"
#інакше
        " -LZ4"
#endif

#якщо HAVE_XZ
        " +XZ"
#інакше
        " -XZ"
#endif

#якщо HAVE_ZLIB
        " +ZLIB"
#інакше
        " -ZLIB"
#endif

#якщо HAVE_ZSTD
        " +ZSTD"
#інакше
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#якщо BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#інакше
        " -BPF_FRAMEWORK"
#endif

#якщо HAVE_VMLINUX_H
        " +BTF"
#інакше
        " -BTF"
#endif

#якщо HAVE_XKBCOMMON
        " +XKBCOMMON"
#інакше
        " -XKBCOMMON"
#endif

#якщо ENABLE_UTMP
        " +UTMP"
#інакше
        " -UTMP"
#endif

#якщо HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#інакше
        " -LIBARCHIVE"
#endif

#якщо EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#інакше
        " -EXPERIMENTAL"
#endif
        ;

static char* systemd_features_with_color(void) {
        const char *p = systemd_features;
        _cleanup_free_ char *ret = NULL;
        ціле r;

        для (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                якщо (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        повернути NULL;
                }
                якщо (r == 0)
                        повернути TAKE_PTR(ret);

                якщо (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        повернути NULL;
                }

                якщо (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                інакше якщо (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                інакше
                        q = strextend(&ret, word);
                якщо (!q) {
                        log_oom_warning();
                        повернути NULL;
                }
        }
}

ціле version(void) {
        _cleanup_free_ char *b = NULL;

        якщо (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        повернути 0;
}

ціле version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        повернути 0;
}

bool experimental_enabled(void) {
#якщо EXPERIMENTAL == EXPERIMENTAL_DISABLED
        повернути false;
#інакше
        ціле r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        якщо (r >= 0)
                повернути r;
        якщо (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        повернути EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
