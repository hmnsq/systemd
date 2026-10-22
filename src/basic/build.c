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

#ܐܢ HAVE_PAM
        "+PAM"
#ܐܚܪܢܐ
        "-PAM"
#endif

#ܐܢ HAVE_AUDIT
        " +AUDIT"
#ܐܚܪܢܐ
        " -AUDIT"
#endif

#ܐܢ HAVE_SELINUX
        " +SELINUX"
#ܐܚܪܢܐ
        " -SELINUX"
#endif

#ܐܢ HAVE_APPARMOR
        " +APPARMOR"
#ܐܚܪܢܐ
        " -APPARMOR"
#endif

#ܐܢ ENABLE_IMA
        " +IMA"
#ܐܚܪܢܐ
        " -IMA"
#endif

#ܐܢ ENABLE_IPE
        " +IPE"
#ܐܚܪܢܐ
        " -IPE"
#endif

#ܐܢ ENABLE_SMACK
        " +SMACK"
#ܐܚܪܢܐ
        " -SMACK"
#endif

#ܐܢ HAVE_SECCOMP
        " +SECCOMP"
#ܐܚܪܢܐ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ܐܢ HAVE_GNUTLS
        " +GNUTLS"
#ܐܚܪܢܐ
        " -GNUTLS"
#endif

#ܐܢ HAVE_OPENSSL
        " +OPENSSL"
#ܐܚܪܢܐ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ܐܢ HAVE_ACL
        " +ACL"
#ܐܚܪܢܐ
        " -ACL"
#endif

#ܐܢ HAVE_BLKID
        " +BLKID"
#ܐܚܪܢܐ
        " -BLKID"
#endif

#ܐܢ HAVE_LIBCURL
        " +CURL"
#ܐܚܪܢܐ
        " -CURL"
#endif

#ܐܢ HAVE_ELFUTILS
        " +ELFUTILS"
#ܐܚܪܢܐ
        " -ELFUTILS"
#endif

#ܐܢ HAVE_LIBFIDO2
        " +FIDO2"
#ܐܚܪܢܐ
        " -FIDO2"
#endif

#ܐܢ HAVE_LIBIDN2
        " +IDN2"
#ܐܚܪܢܐ
        " -IDN2"
#endif

#ܐܢ HAVE_KMOD
        " +KMOD"
#ܐܚܪܢܐ
        " -KMOD"
#endif

#ܐܢ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ܐܚܪܢܐ
        " -LIBCRYPTSETUP"
#endif

#ܐܢ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ܐܚܪܢܐ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ܐܢ HAVE_LIBFDISK
        " +LIBFDISK"
#ܐܚܪܢܐ
        " -LIBFDISK"
#endif

#ܐܢ HAVE_PCRE2
        " +PCRE2"
#ܐܚܪܢܐ
        " -PCRE2"
#endif

#ܐܢ HAVE_PWQUALITY
        " +PWQUALITY"
#ܐܚܪܢܐ
        " -PWQUALITY"
#endif

#ܐܢ HAVE_P11KIT
        " +P11KIT"
#ܐܚܪܢܐ
        " -P11KIT"
#endif

#ܐܢ HAVE_QRENCODE
        " +QRENCODE"
#ܐܚܪܢܐ
        " -QRENCODE"
#endif

#ܐܢ HAVE_TPM2
        " +TPM2"
#ܐܚܪܢܐ
        " -TPM2"
#endif

        /* compressors */

#ܐܢ HAVE_BZIP2
        " +BZIP2"
#ܐܚܪܢܐ
        " -BZIP2"
#endif

#ܐܢ HAVE_LZ4
        " +LZ4"
#ܐܚܪܢܐ
        " -LZ4"
#endif

#ܐܢ HAVE_XZ
        " +XZ"
#ܐܚܪܢܐ
        " -XZ"
#endif

#ܐܢ HAVE_ZLIB
        " +ZLIB"
#ܐܚܪܢܐ
        " -ZLIB"
#endif

#ܐܢ HAVE_ZSTD
        " +ZSTD"
#ܐܚܪܢܐ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ܐܢ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ܐܚܪܢܐ
        " -BPF_FRAMEWORK"
#endif

#ܐܢ HAVE_VMLINUX_H
        " +BTF"
#ܐܚܪܢܐ
        " -BTF"
#endif

#ܐܢ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ܐܚܪܢܐ
        " -XKBCOMMON"
#endif

#ܐܢ ENABLE_UTMP
        " +UTMP"
#ܐܚܪܢܐ
        " -UTMP"
#endif

#ܐܢ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ܐܚܪܢܐ
        " -LIBARCHIVE"
#endif

#ܐܢ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وإلاإذا EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ܐܚܪܢܐ
        " -EXPERIMENTAL"
#endif
        ;

ساكن حرف* systemd_features_with_color(فراغ) {
        ثابت حرف *p = systemd_features;
        _cleanup_free_ حرف *ret = عدم;
        صحيح r;

        لكل (;;) {
                _cleanup_free_ حرف *word = عدم;
                حرف *q;

                r = extract_first_word(&p, &word, عدم, 0);
                ܐܢ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ܦܢܐ عدم;
                }
                ܐܢ (r == 0)
                        ܦܢܐ TAKE_PTR(ret);

                ܐܢ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ܦܢܐ عدم;
                }

                ܐܢ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ܐܚܪܢܐ ܐܢ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ܐܚܪܢܐ
                        q = strextend(&ret, word);
                ܐܢ (!q) {
                        log_oom_warning();
                        ܦܢܐ عدم;
                }
        }
}

صحيح version(فراغ) {
        _cleanup_free_ حرف *b = عدم;

        ܐܢ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ܦܢܐ 0;
}

صحيح version_only(فراغ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ܦܢܐ 0;
}

bool experimental_enabled(فراغ) {
#ܐܢ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ܦܢܐ false;
#ܐܚܪܢܐ
        صحيح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ܐܢ (r >= 0)
                ܦܢܐ r;
        ܐܢ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ܦܢܐ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
