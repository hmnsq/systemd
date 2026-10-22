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

ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ systemd_features =

        /* PAM and MAC frameworks */

#ಯದಿ HAVE_PAM
        "+PAM"
#ಅನ್ಯಥಾ
        "-PAM"
#endif

#ಯದಿ HAVE_AUDIT
        " +AUDIT"
#ಅನ್ಯಥಾ
        " -AUDIT"
#endif

#ಯದಿ HAVE_SELINUX
        " +SELINUX"
#ಅನ್ಯಥಾ
        " -SELINUX"
#endif

#ಯದಿ HAVE_APPARMOR
        " +APPARMOR"
#ಅನ್ಯಥಾ
        " -APPARMOR"
#endif

#ಯದಿ ENABLE_IMA
        " +IMA"
#ಅನ್ಯಥಾ
        " -IMA"
#endif

#ಯದಿ ENABLE_IPE
        " +IPE"
#ಅನ್ಯಥಾ
        " -IPE"
#endif

#ಯದಿ ENABLE_SMACK
        " +SMACK"
#ಅನ್ಯಥಾ
        " -SMACK"
#endif

#ಯದಿ HAVE_SECCOMP
        " +SECCOMP"
#ಅನ್ಯಥಾ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ಯದಿ HAVE_GNUTLS
        " +GNUTLS"
#ಅನ್ಯಥಾ
        " -GNUTLS"
#endif

#ಯದಿ HAVE_OPENSSL
        " +OPENSSL"
#ಅನ್ಯಥಾ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ಯದಿ HAVE_ACL
        " +ACL"
#ಅನ್ಯಥಾ
        " -ACL"
#endif

#ಯದಿ HAVE_BLKID
        " +BLKID"
#ಅನ್ಯಥಾ
        " -BLKID"
#endif

#ಯದಿ HAVE_LIBCURL
        " +CURL"
#ಅನ್ಯಥಾ
        " -CURL"
#endif

#ಯದಿ HAVE_ELFUTILS
        " +ELFUTILS"
#ಅನ್ಯಥಾ
        " -ELFUTILS"
#endif

#ಯದಿ HAVE_LIBFIDO2
        " +FIDO2"
#ಅನ್ಯಥಾ
        " -FIDO2"
#endif

#ಯದಿ HAVE_LIBIDN2
        " +IDN2"
#ಅನ್ಯಥಾ
        " -IDN2"
#endif

#ಯದಿ HAVE_KMOD
        " +KMOD"
#ಅನ್ಯಥಾ
        " -KMOD"
#endif

#ಯದಿ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ಅನ್ಯಥಾ
        " -LIBCRYPTSETUP"
#endif

#ಯದಿ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ಅನ್ಯಥಾ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ಯದಿ HAVE_LIBFDISK
        " +LIBFDISK"
#ಅನ್ಯಥಾ
        " -LIBFDISK"
#endif

#ಯದಿ HAVE_PCRE2
        " +PCRE2"
#ಅನ್ಯಥಾ
        " -PCRE2"
#endif

#ಯದಿ HAVE_PWQUALITY
        " +PWQUALITY"
#ಅನ್ಯಥಾ
        " -PWQUALITY"
#endif

#ಯದಿ HAVE_P11KIT
        " +P11KIT"
#ಅನ್ಯಥಾ
        " -P11KIT"
#endif

#ಯದಿ HAVE_QRENCODE
        " +QRENCODE"
#ಅನ್ಯಥಾ
        " -QRENCODE"
#endif

#ಯದಿ HAVE_TPM2
        " +TPM2"
#ಅನ್ಯಥಾ
        " -TPM2"
#endif

        /* compressors */

#ಯದಿ HAVE_BZIP2
        " +BZIP2"
#ಅನ್ಯಥಾ
        " -BZIP2"
#endif

#ಯದಿ HAVE_LZ4
        " +LZ4"
#ಅನ್ಯಥಾ
        " -LZ4"
#endif

#ಯದಿ HAVE_XZ
        " +XZ"
#ಅನ್ಯಥಾ
        " -XZ"
#endif

#ಯದಿ HAVE_ZLIB
        " +ZLIB"
#ಅನ್ಯಥಾ
        " -ZLIB"
#endif

#ಯದಿ HAVE_ZSTD
        " +ZSTD"
#ಅನ್ಯಥಾ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ಯದಿ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ಅನ್ಯಥಾ
        " -BPF_FRAMEWORK"
#endif

#ಯದಿ HAVE_VMLINUX_H
        " +BTF"
#ಅನ್ಯಥಾ
        " -BTF"
#endif

#ಯದಿ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ಅನ್ಯಥಾ
        " -XKBCOMMON"
#endif

#ಯದಿ ENABLE_UTMP
        " +UTMP"
#ಅನ್ಯಥಾ
        " -UTMP"
#endif

#ಯದಿ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ಅನ್ಯಥಾ
        " -LIBARCHIVE"
#endif

#ಯದಿ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#ನೋಚೇದ್ಯದಿ EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ಅನ್ಯಥಾ
        " -EXPERIMENTAL"
#endif
        ;

ಸ್ಥಿರ ವರ್ಣ* systemd_features_with_color(ಶೂನ್ಯ) {
        ಧ್ರುವ ವರ್ಣ *p = systemd_features;
        _cleanup_free_ ವರ್ಣ *ret = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *word = ರಿಕ್ತ;
                ವರ್ಣ *q;

                r = extract_first_word(&p, &word, ರಿಕ್ತ, 0);
                ಯದಿ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(ret);

                ಯದಿ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }

                ಯದಿ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ಅನ್ಯಥಾ ಯದಿ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ಅನ್ಯಥಾ
                        q = strextend(&ret, word);
                ಯದಿ (!q) {
                        log_oom_warning();
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
        }
}

ಪೂರ್ಣಾಙ್ಕ version(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *b = ರಿಕ್ತ;

        ಯದಿ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ version_only(ಶೂನ್ಯ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool experimental_enabled(ಶೂನ್ಯ) {
#ಯದಿ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ಪ್ರತ್ಯಾಗಮ false;
#ಅನ್ಯಥಾ
        ಪೂರ್ಣಾಙ್ಕ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ಪ್ರತ್ಯಾಗಮ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
