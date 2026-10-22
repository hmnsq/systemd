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
#جيڪڏهن_پورو

#إذا HAVE_AUDIT
        " +AUDIT"
#وإلا
        " -AUDIT"
#جيڪڏهن_پورو

#إذا HAVE_SELINUX
        " +SELINUX"
#وإلا
        " -SELINUX"
#جيڪڏهن_پورو

#إذا HAVE_APPARMOR
        " +APPARMOR"
#وإلا
        " -APPARMOR"
#جيڪڏهن_پورو

#إذا ENABLE_IMA
        " +IMA"
#وإلا
        " -IMA"
#جيڪڏهن_پورو

#إذا ENABLE_IPE
        " +IPE"
#وإلا
        " -IPE"
#جيڪڏهن_پورو

#إذا ENABLE_SMACK
        " +SMACK"
#وإلا
        " -SMACK"
#جيڪڏهن_پورو

#إذا HAVE_SECCOMP
        " +SECCOMP"
#وإلا
        " -SECCOMP"
#جيڪڏهن_پورو

        /* cryptographic libraries */

#إذا HAVE_GNUTLS
        " +GNUTLS"
#وإلا
        " -GNUTLS"
#جيڪڏهن_پورو

#إذا HAVE_OPENSSL
        " +OPENSSL"
#وإلا
        " -OPENSSL"
#جيڪڏهن_پورو

        /* all other libraries, sorted alphabetically */

#إذا HAVE_ACL
        " +ACL"
#وإلا
        " -ACL"
#جيڪڏهن_پورو

#إذا HAVE_BLKID
        " +BLKID"
#وإلا
        " -BLKID"
#جيڪڏهن_پورو

#إذا HAVE_LIBCURL
        " +CURL"
#وإلا
        " -CURL"
#جيڪڏهن_پورو

#إذا HAVE_ELFUTILS
        " +ELFUTILS"
#وإلا
        " -ELFUTILS"
#جيڪڏهن_پورو

#إذا HAVE_LIBFIDO2
        " +FIDO2"
#وإلا
        " -FIDO2"
#جيڪڏهن_پورو

#إذا HAVE_LIBIDN2
        " +IDN2"
#وإلا
        " -IDN2"
#جيڪڏهن_پورو

#إذا HAVE_KMOD
        " +KMOD"
#وإلا
        " -KMOD"
#جيڪڏهن_پورو

#إذا HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#وإلا
        " -LIBCRYPTSETUP"
#جيڪڏهن_پورو

#إذا HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#وإلا
        " -LIBCRYPTSETUP_PLUGINS"
#جيڪڏهن_پورو

#إذا HAVE_LIBFDISK
        " +LIBFDISK"
#وإلا
        " -LIBFDISK"
#جيڪڏهن_پورو

#إذا HAVE_PCRE2
        " +PCRE2"
#وإلا
        " -PCRE2"
#جيڪڏهن_پورو

#إذا HAVE_PWQUALITY
        " +PWQUALITY"
#وإلا
        " -PWQUALITY"
#جيڪڏهن_پورو

#إذا HAVE_P11KIT
        " +P11KIT"
#وإلا
        " -P11KIT"
#جيڪڏهن_پورو

#إذا HAVE_QRENCODE
        " +QRENCODE"
#وإلا
        " -QRENCODE"
#جيڪڏهن_پورو

#إذا HAVE_TPM2
        " +TPM2"
#وإلا
        " -TPM2"
#جيڪڏهن_پورو

        /* compressors */

#إذا HAVE_BZIP2
        " +BZIP2"
#وإلا
        " -BZIP2"
#جيڪڏهن_پورو

#إذا HAVE_LZ4
        " +LZ4"
#وإلا
        " -LZ4"
#جيڪڏهن_پورو

#إذا HAVE_XZ
        " +XZ"
#وإلا
        " -XZ"
#جيڪڏهن_پورو

#إذا HAVE_ZLIB
        " +ZLIB"
#وإلا
        " -ZLIB"
#جيڪڏهن_پورو

#إذا HAVE_ZSTD
        " +ZSTD"
#وإلا
        " -ZSTD"
#جيڪڏهن_پورو

        /* other stuff that doesn't fit above */

#إذا BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#وإلا
        " -BPF_FRAMEWORK"
#جيڪڏهن_پورو

#إذا HAVE_VMLINUX_H
        " +BTF"
#وإلا
        " -BTF"
#جيڪڏهن_پورو

#إذا HAVE_XKBCOMMON
        " +XKBCOMMON"
#وإلا
        " -XKBCOMMON"
#جيڪڏهن_پورو

#إذا ENABLE_UTMP
        " +UTMP"
#وإلا
        " -UTMP"
#جيڪڏهن_پورو

#إذا HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#وإلا
        " -LIBARCHIVE"
#جيڪڏهن_پورو

#إذا EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وإلاإذا EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#وإلا
        " -EXPERIMENTAL"
#جيڪڏهن_پورو
        ;

ساكن حرف* systemd_features_with_color(فراغ) {
        ثابت حرف *p = systemd_features;
        _cleanup_free_ حرف *ret = عدم;
        صحيح r;

        لكل (;;) {
                _cleanup_free_ حرف *لفظ = عدم;
                حرف *q;

                r = ڪڍو_پهريون_لفظ(&p, &لفظ, عدم, 0);
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

                إذا (لفظ[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_منزل_STR(لفظ[0]), ANSI_GREEN, لفظ+1, ANSI_عام);
                وإلا إذا (لفظ[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_منزل_STR(لفظ[0]), ANSI_RED, لفظ+1, ANSI_عام);
                وإلا
                        q = strextend(&ret, لفظ);
                إذا (!q) {
                        log_oom_warning();
                        أرجع عدم;
                }
        }
}

صحيح نسخو(فراغ) {
        _cleanup_free_ حرف *b = عدم;

        إذا (colors_enabled())
                b = systemd_features_with_color();

        لکو("%ssystemd %i%s (" GIT_نسخو ")\n%s\n",
               ansi_highlight(), PROJECT_نسخو, ansi_عام(),
               b ?: systemd_features);
        أرجع 0;
}

صحيح نسخو_only(فراغ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_نسخو_STR);
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
#جيڪڏهن_پورو
}
