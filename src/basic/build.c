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

#nếu HAVE_PAM
        "+PAM"
#không_thì
        "-PAM"
#endif

#nếu HAVE_AUDIT
        " +AUDIT"
#không_thì
        " -AUDIT"
#endif

#nếu HAVE_SELINUX
        " +SELINUX"
#không_thì
        " -SELINUX"
#endif

#nếu HAVE_APPARMOR
        " +APPARMOR"
#không_thì
        " -APPARMOR"
#endif

#nếu ENABLE_IMA
        " +IMA"
#không_thì
        " -IMA"
#endif

#nếu ENABLE_IPE
        " +IPE"
#không_thì
        " -IPE"
#endif

#nếu ENABLE_SMACK
        " +SMACK"
#không_thì
        " -SMACK"
#endif

#nếu HAVE_SECCOMP
        " +SECCOMP"
#không_thì
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#nếu HAVE_GNUTLS
        " +GNUTLS"
#không_thì
        " -GNUTLS"
#endif

#nếu HAVE_OPENSSL
        " +OPENSSL"
#không_thì
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#nếu HAVE_ACL
        " +ACL"
#không_thì
        " -ACL"
#endif

#nếu HAVE_BLKID
        " +BLKID"
#không_thì
        " -BLKID"
#endif

#nếu HAVE_LIBCURL
        " +CURL"
#không_thì
        " -CURL"
#endif

#nếu HAVE_ELFUTILS
        " +ELFUTILS"
#không_thì
        " -ELFUTILS"
#endif

#nếu HAVE_LIBFIDO2
        " +FIDO2"
#không_thì
        " -FIDO2"
#endif

#nếu HAVE_LIBIDN2
        " +IDN2"
#không_thì
        " -IDN2"
#endif

#nếu HAVE_KMOD
        " +KMOD"
#không_thì
        " -KMOD"
#endif

#nếu HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#không_thì
        " -LIBCRYPTSETUP"
#endif

#nếu HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#không_thì
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#nếu HAVE_LIBFDISK
        " +LIBFDISK"
#không_thì
        " -LIBFDISK"
#endif

#nếu HAVE_PCRE2
        " +PCRE2"
#không_thì
        " -PCRE2"
#endif

#nếu HAVE_PWQUALITY
        " +PWQUALITY"
#không_thì
        " -PWQUALITY"
#endif

#nếu HAVE_P11KIT
        " +P11KIT"
#không_thì
        " -P11KIT"
#endif

#nếu HAVE_QRENCODE
        " +QRENCODE"
#không_thì
        " -QRENCODE"
#endif

#nếu HAVE_TPM2
        " +TPM2"
#không_thì
        " -TPM2"
#endif

        /* compressors */

#nếu HAVE_BZIP2
        " +BZIP2"
#không_thì
        " -BZIP2"
#endif

#nếu HAVE_LZ4
        " +LZ4"
#không_thì
        " -LZ4"
#endif

#nếu HAVE_XZ
        " +XZ"
#không_thì
        " -XZ"
#endif

#nếu HAVE_ZLIB
        " +ZLIB"
#không_thì
        " -ZLIB"
#endif

#nếu HAVE_ZSTD
        " +ZSTD"
#không_thì
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#nếu BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#không_thì
        " -BPF_FRAMEWORK"
#endif

#nếu HAVE_VMLINUX_H
        " +BTF"
#không_thì
        " -BTF"
#endif

#nếu HAVE_XKBCOMMON
        " +XKBCOMMON"
#không_thì
        " -XKBCOMMON"
#endif

#nếu ENABLE_UTMP
        " +UTMP"
#không_thì
        " -UTMP"
#endif

#nếu HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#không_thì
        " -LIBARCHIVE"
#endif

#nếu EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#không_thì
        " -EXPERIMENTAL"
#endif
        ;

static char* systemd_features_with_color(void) {
        const char *p = systemd_features;
        _cleanup_free_ char *ret = NULL;
        số_nguyên r;

        cho (;;) {
                _cleanup_free_ char *word = NULL;
                char *q;

                r = extract_first_word(&p, &word, NULL, 0);
                nếu (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        trả_về NULL;
                }
                nếu (r == 0)
                        trả_về TAKE_PTR(ret);

                nếu (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        trả_về NULL;
                }

                nếu (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                không_thì nếu (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                không_thì
                        q = strextend(&ret, word);
                nếu (!q) {
                        log_oom_warning();
                        trả_về NULL;
                }
        }
}

số_nguyên version(void) {
        _cleanup_free_ char *b = NULL;

        nếu (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        trả_về 0;
}

số_nguyên version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        trả_về 0;
}

bool experimental_enabled(void) {
#nếu EXPERIMENTAL == EXPERIMENTAL_DISABLED
        trả_về false;
#không_thì
        số_nguyên r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        nếu (r >= 0)
                trả_về r;
        nếu (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        trả_về EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
