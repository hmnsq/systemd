/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "ansi-color.h"
#شامل "build.h"
#شامل "env-util.h"
#شامل "extract-word.h"
#شامل "log.h"
#شامل "string-util.h"
#شامل "version.h"

ثابت حرف* ثابت systemd_features =

        /* PAM and MAC frameworks */

#اگر HAVE_PAM
        "+PAM"
#ورنہ
        "-PAM"
#اگر_ختم

#اگر HAVE_AUDIT
        " +AUDIT"
#ورنہ
        " -AUDIT"
#اگر_ختم

#اگر HAVE_SELINUX
        " +SELINUX"
#ورنہ
        " -SELINUX"
#اگر_ختم

#اگر HAVE_APPARMOR
        " +APPARMOR"
#ورنہ
        " -APPARMOR"
#اگر_ختم

#اگر ENABLE_IMA
        " +IMA"
#ورنہ
        " -IMA"
#اگر_ختم

#اگر ENABLE_IPE
        " +IPE"
#ورنہ
        " -IPE"
#اگر_ختم

#اگر ENABLE_SMACK
        " +SMACK"
#ورنہ
        " -SMACK"
#اگر_ختم

#اگر HAVE_SECCOMP
        " +SECCOMP"
#ورنہ
        " -SECCOMP"
#اگر_ختم

        /* cryptographic libraries */

#اگر HAVE_GNUTLS
        " +GNUTLS"
#ورنہ
        " -GNUTLS"
#اگر_ختم

#اگر HAVE_OPENSSL
        " +OPENSSL"
#ورنہ
        " -OPENSSL"
#اگر_ختم

        /* all other libraries, sorted alphabetically */

#اگر HAVE_ACL
        " +ACL"
#ورنہ
        " -ACL"
#اگر_ختم

#اگر HAVE_BLKID
        " +BLKID"
#ورنہ
        " -BLKID"
#اگر_ختم

#اگر HAVE_LIBCURL
        " +CURL"
#ورنہ
        " -CURL"
#اگر_ختم

#اگر HAVE_ELFUTILS
        " +ELFUTILS"
#ورنہ
        " -ELFUTILS"
#اگر_ختم

#اگر HAVE_LIBFIDO2
        " +FIDO2"
#ورنہ
        " -FIDO2"
#اگر_ختم

#اگر HAVE_LIBIDN2
        " +IDN2"
#ورنہ
        " -IDN2"
#اگر_ختم

#اگر HAVE_KMOD
        " +KMOD"
#ورنہ
        " -KMOD"
#اگر_ختم

#اگر HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ورنہ
        " -LIBCRYPTSETUP"
#اگر_ختم

#اگر HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ورنہ
        " -LIBCRYPTSETUP_PLUGINS"
#اگر_ختم

#اگر HAVE_LIBFDISK
        " +LIBFDISK"
#ورنہ
        " -LIBFDISK"
#اگر_ختم

#اگر HAVE_PCRE2
        " +PCRE2"
#ورنہ
        " -PCRE2"
#اگر_ختم

#اگر HAVE_PWQUALITY
        " +PWQUALITY"
#ورنہ
        " -PWQUALITY"
#اگر_ختم

#اگر HAVE_P11KIT
        " +P11KIT"
#ورنہ
        " -P11KIT"
#اگر_ختم

#اگر HAVE_QRENCODE
        " +QRENCODE"
#ورنہ
        " -QRENCODE"
#اگر_ختم

#اگر HAVE_TPM2
        " +TPM2"
#ورنہ
        " -TPM2"
#اگر_ختم

        /* compressors */

#اگر HAVE_BZIP2
        " +BZIP2"
#ورنہ
        " -BZIP2"
#اگر_ختم

#اگر HAVE_LZ4
        " +LZ4"
#ورنہ
        " -LZ4"
#اگر_ختم

#اگر HAVE_XZ
        " +XZ"
#ورنہ
        " -XZ"
#اگر_ختم

#اگر HAVE_ZLIB
        " +ZLIB"
#ورنہ
        " -ZLIB"
#اگر_ختم

#اگر HAVE_ZSTD
        " +ZSTD"
#ورنہ
        " -ZSTD"
#اگر_ختم

        /* other stuff that doesn't fit above */

#اگر BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ورنہ
        " -BPF_FRAMEWORK"
#اگر_ختم

#اگر HAVE_VMLINUX_H
        " +BTF"
#ورنہ
        " -BTF"
#اگر_ختم

#اگر HAVE_XKBCOMMON
        " +XKBCOMMON"
#ورنہ
        " -XKBCOMMON"
#اگر_ختم

#اگر ENABLE_UTMP
        " +UTMP"
#ورنہ
        " -UTMP"
#اگر_ختم

#اگر HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ورنہ
        " -LIBARCHIVE"
#اگر_ختم

#اگر EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وگرنہ_اگر EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ورنہ
        " -EXPERIMENTAL"
#اگر_ختم
        ;

ساکن حرف* systemd_features_with_color(خالی) {
        ثابت حرف *p = systemd_features;
        _cleanup_free_ حرف *ret = تہی;
        صحیح r;

        برائے (;;) {
                _cleanup_free_ حرف *لفظ = تہی;
                حرف *q;

                r = نکالو_پہلا_لفظ(&p, &لفظ, تہی, 0);
                اگر (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        واپس تہی;
                }
                اگر (r == 0)
                        واپس TAKE_PTR(ret);

                اگر (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        واپس تہی;
                }

                اگر (لفظ[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_مقصد_STR(لفظ[0]), ANSI_GREEN, لفظ+1, ANSI_معمول);
                ورنہ اگر (لفظ[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_مقصد_STR(لفظ[0]), ANSI_RED, لفظ+1, ANSI_معمول);
                ورنہ
                        q = strextend(&ret, لفظ);
                اگر (!q) {
                        log_oom_warning();
                        واپس تہی;
                }
        }
}

صحیح نسخہ(خالی) {
        _cleanup_free_ حرف *b = تہی;

        اگر (colors_enabled())
                b = systemd_features_with_color();

        لکھو("%ssystemd %i%s (" GIT_نسخہ ")\n%s\n",
               ansi_highlight(), PROJECT_نسخہ, ansi_معمول(),
               b ?: systemd_features);
        واپس 0;
}

صحیح نسخہ_only(خالی) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_نسخہ_STR);
        واپس 0;
}

bool experimental_enabled(خالی) {
#اگر EXPERIMENTAL == EXPERIMENTAL_DISABLED
        واپس false;
#ورنہ
        صحیح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        اگر (r >= 0)
                واپس r;
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        واپس EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#اگر_ختم
}
