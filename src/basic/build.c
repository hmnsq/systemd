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

#만약 HAVE_PAM
        "+PAM"
#아니면
        "-PAM"
#endif

#만약 HAVE_AUDIT
        " +AUDIT"
#아니면
        " -AUDIT"
#endif

#만약 HAVE_SELINUX
        " +SELINUX"
#아니면
        " -SELINUX"
#endif

#만약 HAVE_APPARMOR
        " +APPARMOR"
#아니면
        " -APPARMOR"
#endif

#만약 ENABLE_IMA
        " +IMA"
#아니면
        " -IMA"
#endif

#만약 ENABLE_IPE
        " +IPE"
#아니면
        " -IPE"
#endif

#만약 ENABLE_SMACK
        " +SMACK"
#아니면
        " -SMACK"
#endif

#만약 HAVE_SECCOMP
        " +SECCOMP"
#아니면
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#만약 HAVE_GNUTLS
        " +GNUTLS"
#아니면
        " -GNUTLS"
#endif

#만약 HAVE_OPENSSL
        " +OPENSSL"
#아니면
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#만약 HAVE_ACL
        " +ACL"
#아니면
        " -ACL"
#endif

#만약 HAVE_BLKID
        " +BLKID"
#아니면
        " -BLKID"
#endif

#만약 HAVE_LIBCURL
        " +CURL"
#아니면
        " -CURL"
#endif

#만약 HAVE_ELFUTILS
        " +ELFUTILS"
#아니면
        " -ELFUTILS"
#endif

#만약 HAVE_LIBFIDO2
        " +FIDO2"
#아니면
        " -FIDO2"
#endif

#만약 HAVE_LIBIDN2
        " +IDN2"
#아니면
        " -IDN2"
#endif

#만약 HAVE_KMOD
        " +KMOD"
#아니면
        " -KMOD"
#endif

#만약 HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#아니면
        " -LIBCRYPTSETUP"
#endif

#만약 HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#아니면
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#만약 HAVE_LIBFDISK
        " +LIBFDISK"
#아니면
        " -LIBFDISK"
#endif

#만약 HAVE_PCRE2
        " +PCRE2"
#아니면
        " -PCRE2"
#endif

#만약 HAVE_PWQUALITY
        " +PWQUALITY"
#아니면
        " -PWQUALITY"
#endif

#만약 HAVE_P11KIT
        " +P11KIT"
#아니면
        " -P11KIT"
#endif

#만약 HAVE_QRENCODE
        " +QRENCODE"
#아니면
        " -QRENCODE"
#endif

#만약 HAVE_TPM2
        " +TPM2"
#아니면
        " -TPM2"
#endif

        /* compressors */

#만약 HAVE_BZIP2
        " +BZIP2"
#아니면
        " -BZIP2"
#endif

#만약 HAVE_LZ4
        " +LZ4"
#아니면
        " -LZ4"
#endif

#만약 HAVE_XZ
        " +XZ"
#아니면
        " -XZ"
#endif

#만약 HAVE_ZLIB
        " +ZLIB"
#아니면
        " -ZLIB"
#endif

#만약 HAVE_ZSTD
        " +ZSTD"
#아니면
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#만약 BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#아니면
        " -BPF_FRAMEWORK"
#endif

#만약 HAVE_VMLINUX_H
        " +BTF"
#아니면
        " -BTF"
#endif

#만약 HAVE_XKBCOMMON
        " +XKBCOMMON"
#아니면
        " -XKBCOMMON"
#endif

#만약 ENABLE_UTMP
        " +UTMP"
#아니면
        " -UTMP"
#endif

#만약 HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#아니면
        " -LIBARCHIVE"
#endif

#만약 EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#아니면
        " -EXPERIMENTAL"
#endif
        ;

static char* systemd_features_with_color(void) {
        const char *p = systemd_features;
        _cleanup_free_ char *ret = NULL;
        정수 r;

        위해 (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                만약 (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        반환 NULL;
                }
                만약 (r == 0)
                        반환 TAKE_PTR(ret);

                만약 (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        반환 NULL;
                }

                만약 (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                아니면 만약 (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                아니면
                        q = strextend(&ret, word);
                만약 (!q) {
                        log_oom_warning();
                        반환 NULL;
                }
        }
}

정수 version(void) {
        _cleanup_free_ char *b = NULL;

        만약 (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        반환 0;
}

정수 version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        반환 0;
}

bool experimental_enabled(void) {
#만약 EXPERIMENTAL == EXPERIMENTAL_DISABLED
        반환 false;
#아니면
        정수 r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        만약 (r >= 0)
                반환 r;
        만약 (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        반환 EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
