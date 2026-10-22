/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ansi-color.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "build.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

ਅਚਲ ਅੱਖਰ* ਅਚਲ systemd_features =

        /* PAM and MAC frameworks */

#ਜੇ HAVE_PAM
        "+PAM"
#ਨਹੀਂਤਾਂ
        "-PAM"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_AUDIT
        " +AUDIT"
#ਨਹੀਂਤਾਂ
        " -AUDIT"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_SELINUX
        " +SELINUX"
#ਨਹੀਂਤਾਂ
        " -SELINUX"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_APPARMOR
        " +APPARMOR"
#ਨਹੀਂਤਾਂ
        " -APPARMOR"
#ਜੇ_ਅੰਤ

#ਜੇ ENABLE_IMA
        " +IMA"
#ਨਹੀਂਤਾਂ
        " -IMA"
#ਜੇ_ਅੰਤ

#ਜੇ ENABLE_IPE
        " +IPE"
#ਨਹੀਂਤਾਂ
        " -IPE"
#ਜੇ_ਅੰਤ

#ਜੇ ENABLE_SMACK
        " +SMACK"
#ਨਹੀਂਤਾਂ
        " -SMACK"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_SECCOMP
        " +SECCOMP"
#ਨਹੀਂਤਾਂ
        " -SECCOMP"
#ਜੇ_ਅੰਤ

        /* cryptographic libraries */

#ਜੇ HAVE_GNUTLS
        " +GNUTLS"
#ਨਹੀਂਤਾਂ
        " -GNUTLS"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_OPENSSL
        " +OPENSSL"
#ਨਹੀਂਤਾਂ
        " -OPENSSL"
#ਜੇ_ਅੰਤ

        /* all other libraries, sorted alphabetically */

#ਜੇ HAVE_ACL
        " +ACL"
#ਨਹੀਂਤਾਂ
        " -ACL"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BLKID
        " +BLKID"
#ਨਹੀਂਤਾਂ
        " -BLKID"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBCURL
        " +CURL"
#ਨਹੀਂਤਾਂ
        " -CURL"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ELFUTILS
        " +ELFUTILS"
#ਨਹੀਂਤਾਂ
        " -ELFUTILS"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBFIDO2
        " +FIDO2"
#ਨਹੀਂਤਾਂ
        " -FIDO2"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBIDN2
        " +IDN2"
#ਨਹੀਂਤਾਂ
        " -IDN2"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_KMOD
        " +KMOD"
#ਨਹੀਂਤਾਂ
        " -KMOD"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ਨਹੀਂਤਾਂ
        " -LIBCRYPTSETUP"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ਨਹੀਂਤਾਂ
        " -LIBCRYPTSETUP_PLUGINS"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBFDISK
        " +LIBFDISK"
#ਨਹੀਂਤਾਂ
        " -LIBFDISK"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_PCRE2
        " +PCRE2"
#ਨਹੀਂਤਾਂ
        " -PCRE2"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_PWQUALITY
        " +PWQUALITY"
#ਨਹੀਂਤਾਂ
        " -PWQUALITY"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_P11KIT
        " +P11KIT"
#ਨਹੀਂਤਾਂ
        " -P11KIT"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_QRENCODE
        " +QRENCODE"
#ਨਹੀਂਤਾਂ
        " -QRENCODE"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_TPM2
        " +TPM2"
#ਨਹੀਂਤਾਂ
        " -TPM2"
#ਜੇ_ਅੰਤ

        /* compressors */

#ਜੇ HAVE_BZIP2
        " +BZIP2"
#ਨਹੀਂਤਾਂ
        " -BZIP2"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        " +LZ4"
#ਨਹੀਂਤਾਂ
        " -LZ4"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_XZ
        " +XZ"
#ਨਹੀਂਤਾਂ
        " -XZ"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        " +ZLIB"
#ਨਹੀਂਤਾਂ
        " -ZLIB"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        " +ZSTD"
#ਨਹੀਂਤਾਂ
        " -ZSTD"
#ਜੇ_ਅੰਤ

        /* other stuff that doesn't fit above */

#ਜੇ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ਨਹੀਂਤਾਂ
        " -BPF_FRAMEWORK"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_VMLINUX_H
        " +BTF"
#ਨਹੀਂਤਾਂ
        " -BTF"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ਨਹੀਂਤਾਂ
        " -XKBCOMMON"
#ਜੇ_ਅੰਤ

#ਜੇ ENABLE_UTMP
        " +UTMP"
#ਨਹੀਂਤਾਂ
        " -UTMP"
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ਨਹੀਂਤਾਂ
        " -LIBARCHIVE"
#ਜੇ_ਅੰਤ

#ਜੇ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#ਜਾਂਜੇ EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ਨਹੀਂਤਾਂ
        " -EXPERIMENTAL"
#ਜੇ_ਅੰਤ
        ;

ਸਥਿਰ ਅੱਖਰ* systemd_features_with_color(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *p = systemd_features;
        _cleanup_free_ ਅੱਖਰ *ret = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸ਼ਬਦ = ਨਿਰਾ;
                ਅੱਖਰ *q;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&p, &ਸ਼ਬਦ, ਨਿਰਾ, 0);
                ਜੇ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ਵਾਪਸ ਨਿਰਾ;
                }
                ਜੇ (r == 0)
                        ਵਾਪਸ TAKE_PTR(ret);

                ਜੇ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ਵਾਪਸ ਨਿਰਾ;
                }

                ਜੇ (ਸ਼ਬਦ[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_ਮੰਜ਼ਿਲ_STR(ਸ਼ਬਦ[0]), ANSI_GREEN, ਸ਼ਬਦ+1, ANSI_ਸਾਧਾਰਨ);
                ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ਬਦ[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_ਮੰਜ਼ਿਲ_STR(ਸ਼ਬਦ[0]), ANSI_RED, ਸ਼ਬਦ+1, ANSI_ਸਾਧਾਰਨ);
                ਨਹੀਂਤਾਂ
                        q = strextend(&ret, ਸ਼ਬਦ);
                ਜੇ (!q) {
                        log_oom_warning();
                        ਵਾਪਸ ਨਿਰਾ;
                }
        }
}

ਪੂਰਨ ਸੰਸਕਰਣ(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *b = ਨਿਰਾ;

        ਜੇ (colors_enabled())
                b = systemd_features_with_color();

        ਲਿਖੋ("%ssystemd %i%s (" GIT_ਸੰਸਕਰਣ ")\n%s\n",
               ansi_highlight(), PROJECT_ਸੰਸਕਰਣ, ansi_ਸਾਧਾਰਨ(),
               b ?: systemd_features);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੰਸਕਰਣ_only(ਖਾਲੀ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_ਸੰਸਕਰਣ_STR);
        ਵਾਪਸ 0;
}

bool experimental_enabled(ਖਾਲੀ) {
#ਜੇ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ਵਾਪਸ false;
#ਨਹੀਂਤਾਂ
        ਪੂਰਨ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ਜੇ (r >= 0)
                ਵਾਪਸ r;
        ਜੇ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ਵਾਪਸ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#ਜੇ_ਅੰਤ
}
