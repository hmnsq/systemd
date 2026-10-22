/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-color.h"
#ಅನ್ತರ್ಭಾವ "build.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "version.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ systemd_features =

        /* PAM and MAC frameworks */

#ಆದರೆ HAVE_PAM
        "+PAM"
#ಇಲ್ಲದಿದ್ದರೆ
        "-PAM"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_AUDIT
        " +AUDIT"
#ಇಲ್ಲದಿದ್ದರೆ
        " -AUDIT"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_SELINUX
        " +SELINUX"
#ಇಲ್ಲದಿದ್ದರೆ
        " -SELINUX"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_APPARMOR
        " +APPARMOR"
#ಇಲ್ಲದಿದ್ದರೆ
        " -APPARMOR"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ ENABLE_IMA
        " +IMA"
#ಇಲ್ಲದಿದ್ದರೆ
        " -IMA"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ ENABLE_IPE
        " +IPE"
#ಇಲ್ಲದಿದ್ದರೆ
        " -IPE"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ ENABLE_SMACK
        " +SMACK"
#ಇಲ್ಲದಿದ್ದರೆ
        " -SMACK"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_SECCOMP
        " +SECCOMP"
#ಇಲ್ಲದಿದ್ದರೆ
        " -SECCOMP"
#ಆದರೆ_ಅಂತ್ಯ

        /* cryptographic libraries */

#ಆದರೆ HAVE_GNUTLS
        " +GNUTLS"
#ಇಲ್ಲದಿದ್ದರೆ
        " -GNUTLS"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_OPENSSL
        " +OPENSSL"
#ಇಲ್ಲದಿದ್ದರೆ
        " -OPENSSL"
#ಆದರೆ_ಅಂತ್ಯ

        /* all other libraries, sorted alphabetically */

#ಆದರೆ HAVE_ACL
        " +ACL"
#ಇಲ್ಲದಿದ್ದರೆ
        " -ACL"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BLKID
        " +BLKID"
#ಇಲ್ಲದಿದ್ದರೆ
        " -BLKID"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBCURL
        " +CURL"
#ಇಲ್ಲದಿದ್ದರೆ
        " -CURL"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ELFUTILS
        " +ELFUTILS"
#ಇಲ್ಲದಿದ್ದರೆ
        " -ELFUTILS"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBFIDO2
        " +FIDO2"
#ಇಲ್ಲದಿದ್ದರೆ
        " -FIDO2"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBIDN2
        " +IDN2"
#ಇಲ್ಲದಿದ್ದರೆ
        " -IDN2"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_KMOD
        " +KMOD"
#ಇಲ್ಲದಿದ್ದರೆ
        " -KMOD"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ಇಲ್ಲದಿದ್ದರೆ
        " -LIBCRYPTSETUP"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ಇಲ್ಲದಿದ್ದರೆ
        " -LIBCRYPTSETUP_PLUGINS"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBFDISK
        " +LIBFDISK"
#ಇಲ್ಲದಿದ್ದರೆ
        " -LIBFDISK"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_PCRE2
        " +PCRE2"
#ಇಲ್ಲದಿದ್ದರೆ
        " -PCRE2"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_PWQUALITY
        " +PWQUALITY"
#ಇಲ್ಲದಿದ್ದರೆ
        " -PWQUALITY"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_P11KIT
        " +P11KIT"
#ಇಲ್ಲದಿದ್ದರೆ
        " -P11KIT"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_QRENCODE
        " +QRENCODE"
#ಇಲ್ಲದಿದ್ದರೆ
        " -QRENCODE"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_TPM2
        " +TPM2"
#ಇಲ್ಲದಿದ್ದರೆ
        " -TPM2"
#ಆದರೆ_ಅಂತ್ಯ

        /* compressors */

#ಆದರೆ HAVE_BZIP2
        " +BZIP2"
#ಇಲ್ಲದಿದ್ದರೆ
        " -BZIP2"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        " +LZ4"
#ಇಲ್ಲದಿದ್ದರೆ
        " -LZ4"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_XZ
        " +XZ"
#ಇಲ್ಲದಿದ್ದರೆ
        " -XZ"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        " +ZLIB"
#ಇಲ್ಲದಿದ್ದರೆ
        " -ZLIB"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        " +ZSTD"
#ಇಲ್ಲದಿದ್ದರೆ
        " -ZSTD"
#ಆದರೆ_ಅಂತ್ಯ

        /* other stuff that doesn't fit above */

#ಆದರೆ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ಇಲ್ಲದಿದ್ದರೆ
        " -BPF_FRAMEWORK"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_VMLINUX_H
        " +BTF"
#ಇಲ್ಲದಿದ್ದರೆ
        " -BTF"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ಇಲ್ಲದಿದ್ದರೆ
        " -XKBCOMMON"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ ENABLE_UTMP
        " +UTMP"
#ಇಲ್ಲದಿದ್ದರೆ
        " -UTMP"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ಇಲ್ಲದಿದ್ದರೆ
        " -LIBARCHIVE"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#ಅಥವಾಆದರೆ EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ಇಲ್ಲದಿದ್ದರೆ
        " -EXPERIMENTAL"
#ಆದರೆ_ಅಂತ್ಯ
        ;

ಸ್ಥಿರ ಅಕ್ಷರ* systemd_features_with_color(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = systemd_features;
        _cleanup_free_ ಅಕ್ಷರ *ret = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಶಬ್ದ = ಖಾಲಿ;
                ಅಕ್ಷರ *q;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&p, &ಶಬ್ದ, ಖಾಲಿ, 0);
                ಆದರೆ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ TAKE_PTR(ret);

                ಆದರೆ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }

                ಆದರೆ (ಶಬ್ದ[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_ಗಮ್ಯ_STR(ಶಬ್ದ[0]), ANSI_GREEN, ಶಬ್ದ+1, ANSI_ಸಾಮಾನ್ಯ);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಶಬ್ದ[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_ಗಮ್ಯ_STR(ಶಬ್ದ[0]), ANSI_RED, ಶಬ್ದ+1, ANSI_ಸಾಮಾನ್ಯ);
                ಇಲ್ಲದಿದ್ದರೆ
                        q = strextend(&ret, ಶಬ್ದ);
                ಆದರೆ (!q) {
                        log_oom_warning();
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
        }
}

ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *b = ಖಾಲಿ;

        ಆದರೆ (colors_enabled())
                b = systemd_features_with_color();

        ಬರೆ("%ssystemd %i%s (" GIT_ಆವೃತ್ತಿ ")\n%s\n",
               ansi_highlight(), PROJECT_ಆವೃತ್ತಿ, ansi_ಸಾಮಾನ್ಯ(),
               b ?: systemd_features);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_only(ಶೂನ್ಯ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_ಆವೃತ್ತಿ_STR);
        ಹಿಂತಿರುಗಿ 0;
}

bool experimental_enabled(ಶೂನ್ಯ) {
#ಆದರೆ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ಹಿಂತಿರುಗಿ false;
#ಇಲ್ಲದಿದ್ದರೆ
        ಪೂರ್ಣಾಂಕ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ಹಿಂತಿರುಗಿ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#ಆದರೆ_ಅಂತ್ಯ
}
