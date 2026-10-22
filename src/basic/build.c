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

#ถ้า HAVE_PAM
        "+PAM"
#มิฉะนั้น
        "-PAM"
#endif

#ถ้า HAVE_AUDIT
        " +AUDIT"
#มิฉะนั้น
        " -AUDIT"
#endif

#ถ้า HAVE_SELINUX
        " +SELINUX"
#มิฉะนั้น
        " -SELINUX"
#endif

#ถ้า HAVE_APPARMOR
        " +APPARMOR"
#มิฉะนั้น
        " -APPARMOR"
#endif

#ถ้า ENABLE_IMA
        " +IMA"
#มิฉะนั้น
        " -IMA"
#endif

#ถ้า ENABLE_IPE
        " +IPE"
#มิฉะนั้น
        " -IPE"
#endif

#ถ้า ENABLE_SMACK
        " +SMACK"
#มิฉะนั้น
        " -SMACK"
#endif

#ถ้า HAVE_SECCOMP
        " +SECCOMP"
#มิฉะนั้น
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ถ้า HAVE_GNUTLS
        " +GNUTLS"
#มิฉะนั้น
        " -GNUTLS"
#endif

#ถ้า HAVE_OPENSSL
        " +OPENSSL"
#มิฉะนั้น
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ถ้า HAVE_ACL
        " +ACL"
#มิฉะนั้น
        " -ACL"
#endif

#ถ้า HAVE_BLKID
        " +BLKID"
#มิฉะนั้น
        " -BLKID"
#endif

#ถ้า HAVE_LIBCURL
        " +CURL"
#มิฉะนั้น
        " -CURL"
#endif

#ถ้า HAVE_ELFUTILS
        " +ELFUTILS"
#มิฉะนั้น
        " -ELFUTILS"
#endif

#ถ้า HAVE_LIBFIDO2
        " +FIDO2"
#มิฉะนั้น
        " -FIDO2"
#endif

#ถ้า HAVE_LIBIDN2
        " +IDN2"
#มิฉะนั้น
        " -IDN2"
#endif

#ถ้า HAVE_KMOD
        " +KMOD"
#มิฉะนั้น
        " -KMOD"
#endif

#ถ้า HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#มิฉะนั้น
        " -LIBCRYPTSETUP"
#endif

#ถ้า HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#มิฉะนั้น
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ถ้า HAVE_LIBFDISK
        " +LIBFDISK"
#มิฉะนั้น
        " -LIBFDISK"
#endif

#ถ้า HAVE_PCRE2
        " +PCRE2"
#มิฉะนั้น
        " -PCRE2"
#endif

#ถ้า HAVE_PWQUALITY
        " +PWQUALITY"
#มิฉะนั้น
        " -PWQUALITY"
#endif

#ถ้า HAVE_P11KIT
        " +P11KIT"
#มิฉะนั้น
        " -P11KIT"
#endif

#ถ้า HAVE_QRENCODE
        " +QRENCODE"
#มิฉะนั้น
        " -QRENCODE"
#endif

#ถ้า HAVE_TPM2
        " +TPM2"
#มิฉะนั้น
        " -TPM2"
#endif

        /* compressors */

#ถ้า HAVE_BZIP2
        " +BZIP2"
#มิฉะนั้น
        " -BZIP2"
#endif

#ถ้า HAVE_LZ4
        " +LZ4"
#มิฉะนั้น
        " -LZ4"
#endif

#ถ้า HAVE_XZ
        " +XZ"
#มิฉะนั้น
        " -XZ"
#endif

#ถ้า HAVE_ZLIB
        " +ZLIB"
#มิฉะนั้น
        " -ZLIB"
#endif

#ถ้า HAVE_ZSTD
        " +ZSTD"
#มิฉะนั้น
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ถ้า BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#มิฉะนั้น
        " -BPF_FRAMEWORK"
#endif

#ถ้า HAVE_VMLINUX_H
        " +BTF"
#มิฉะนั้น
        " -BTF"
#endif

#ถ้า HAVE_XKBCOMMON
        " +XKBCOMMON"
#มิฉะนั้น
        " -XKBCOMMON"
#endif

#ถ้า ENABLE_UTMP
        " +UTMP"
#มิฉะนั้น
        " -UTMP"
#endif

#ถ้า HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#มิฉะนั้น
        " -LIBARCHIVE"
#endif

#ถ้า EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#มิฉะนั้น
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        จำนวนเต็ม r;

        สำหรับ (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                ถ้า (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        คืน NULL;
                }
                ถ้า (r == 0)
                        คืน TAKE_PTR(ret);

                ถ้า (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        คืน NULL;
                }

                ถ้า (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                มิฉะนั้น ถ้า (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                มิฉะนั้น
                        q = strextend(&ret, word);
                ถ้า (!q) {
                        log_oom_warning();
                        คืน NULL;
                }
        }
}

จำนวนเต็ม version(void) {
        _cleanup_free_ var_na *b = NULL;

        ถ้า (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        คืน 0;
}

จำนวนเต็ม version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        คืน 0;
}

bool experimental_enabled(void) {
#ถ้า EXPERIMENTAL == EXPERIMENTAL_DISABLED
        คืน false;
#มิฉะนั้น
        จำนวนเต็ม r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ถ้า (r >= 0)
                คืน r;
        ถ้า (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        คืน EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
