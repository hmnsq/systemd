/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "ansi-color.h"
#ضمن "build.h"
#ضمن "env-util.h"
#ضمن "extract-word.h"
#ضمن "log.h"
#ضمن "string-util.h"
#ضمن "version.h"

ثابت حرف* ثابت systemd_features =

        /* PAM and MAC frameworks */

#إذا HAVE_PAM
        "+PAM"
#وإلا
        "-PAM"
#پایان_اگر

#إذا HAVE_AUDIT
        " +AUDIT"
#وإلا
        " -AUDIT"
#پایان_اگر

#إذا HAVE_SELINUX
        " +SELINUX"
#وإلا
        " -SELINUX"
#پایان_اگر

#إذا HAVE_APPARMOR
        " +APPARMOR"
#وإلا
        " -APPARMOR"
#پایان_اگر

#إذا ENABLE_IMA
        " +IMA"
#وإلا
        " -IMA"
#پایان_اگر

#إذا ENABLE_IPE
        " +IPE"
#وإلا
        " -IPE"
#پایان_اگر

#إذا ENABLE_SMACK
        " +SMACK"
#وإلا
        " -SMACK"
#پایان_اگر

#إذا HAVE_SECCOMP
        " +SECCOMP"
#وإلا
        " -SECCOMP"
#پایان_اگر

        /* cryptographic libraries */

#إذا HAVE_GNUTLS
        " +GNUTLS"
#وإلا
        " -GNUTLS"
#پایان_اگر

#إذا HAVE_OPENSSL
        " +OPENSSL"
#وإلا
        " -OPENSSL"
#پایان_اگر

        /* all other libraries, sorted alphabetically */

#إذا HAVE_ACL
        " +ACL"
#وإلا
        " -ACL"
#پایان_اگر

#إذا HAVE_BLKID
        " +BLKID"
#وإلا
        " -BLKID"
#پایان_اگر

#إذا HAVE_LIBCURL
        " +CURL"
#وإلا
        " -CURL"
#پایان_اگر

#إذا HAVE_ELFUTILS
        " +ELFUTILS"
#وإلا
        " -ELFUTILS"
#پایان_اگر

#إذا HAVE_LIBFIDO2
        " +FIDO2"
#وإلا
        " -FIDO2"
#پایان_اگر

#إذا HAVE_LIBIDN2
        " +IDN2"
#وإلا
        " -IDN2"
#پایان_اگر

#إذا HAVE_KMOD
        " +KMOD"
#وإلا
        " -KMOD"
#پایان_اگر

#إذا HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#وإلا
        " -LIBCRYPTSETUP"
#پایان_اگر

#إذا HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#وإلا
        " -LIBCRYPTSETUP_PLUGINS"
#پایان_اگر

#إذا HAVE_LIBFDISK
        " +LIBFDISK"
#وإلا
        " -LIBFDISK"
#پایان_اگر

#إذا HAVE_PCRE2
        " +PCRE2"
#وإلا
        " -PCRE2"
#پایان_اگر

#إذا HAVE_PWQUALITY
        " +PWQUALITY"
#وإلا
        " -PWQUALITY"
#پایان_اگر

#إذا HAVE_P11KIT
        " +P11KIT"
#وإلا
        " -P11KIT"
#پایان_اگر

#إذا HAVE_QRENCODE
        " +QRENCODE"
#وإلا
        " -QRENCODE"
#پایان_اگر

#إذا HAVE_TPM2
        " +TPM2"
#وإلا
        " -TPM2"
#پایان_اگر

        /* compressors */

#إذا HAVE_BZIP2
        " +BZIP2"
#وإلا
        " -BZIP2"
#پایان_اگر

#إذا HAVE_LZ4
        " +LZ4"
#وإلا
        " -LZ4"
#پایان_اگر

#إذا HAVE_XZ
        " +XZ"
#وإلا
        " -XZ"
#پایان_اگر

#إذا HAVE_ZLIB
        " +ZLIB"
#وإلا
        " -ZLIB"
#پایان_اگر

#إذا HAVE_ZSTD
        " +ZSTD"
#وإلا
        " -ZSTD"
#پایان_اگر

        /* other stuff that doesn't fit above */

#إذا BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#وإلا
        " -BPF_FRAMEWORK"
#پایان_اگر

#إذا HAVE_VMLINUX_H
        " +BTF"
#وإلا
        " -BTF"
#پایان_اگر

#إذا HAVE_XKBCOMMON
        " +XKBCOMMON"
#وإلا
        " -XKBCOMMON"
#پایان_اگر

#إذا ENABLE_UTMP
        " +UTMP"
#وإلا
        " -UTMP"
#پایان_اگر

#إذا HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#وإلا
        " -LIBARCHIVE"
#پایان_اگر

#إذا EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وإلاإذا EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#وإلا
        " -EXPERIMENTAL"
#پایان_اگر
        ;

ساكن حرف* systemd_features_with_color(فراغ) {
        ثابت حرف *p = systemd_features;
        _cleanup_free_ حرف *ret = عدم;
        صحيح r;

        لكل (;;) {
                _cleanup_free_ حرف *کلمه = عدم;
                حرف *q;

                r = استخراج_اولی_کلمه(&p, &کلمه, عدم, 0);
                إذا (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        أرجع عدم;
                }
                إذا (r == 0)
                        أرجع TAKE_PTR(ret);

                إذا (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        أرجع عدم;
                }

                إذا (کلمه[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_مقصد_STR(کلمه[0]), ANSI_GREEN, کلمه+1, ANSI_نرمال);
                وإلا إذا (کلمه[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_مقصد_STR(کلمه[0]), ANSI_RED, کلمه+1, ANSI_نرمال);
                وإلا
                        q = strextend(&ret, کلمه);
                إذا (!q) {
                        log_oom_warning();
                        أرجع عدم;
                }
        }
}

صحيح نسخه(فراغ) {
        _cleanup_free_ حرف *b = عدم;

        إذا (colors_enabled())
                b = systemd_features_with_color();

        چاپ_کن("%ssystemd %i%s (" GIT_نسخه ")\n%s\n",
               ansi_highlight(), PROJECT_نسخه, ansi_نرمال(),
               b ?: systemd_features);
        أرجع 0;
}

صحيح نسخه_only(فراغ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_نسخه_STR);
        أرجع 0;
}

bool experimental_enabled(فراغ) {
#إذا EXPERIMENTAL == EXPERIMENTAL_DISABLED
        أرجع false;
#وإلا
        صحيح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        إذا (r >= 0)
                أرجع r;
        إذا (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        أرجع EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#پایان_اگر
}
