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
#endif

#إذا HAVE_AUDIT
        " +AUDIT"
#وإلا
        " -AUDIT"
#endif

#إذا HAVE_SELINUX
        " +SELINUX"
#وإلا
        " -SELINUX"
#endif

#إذا HAVE_APPARMOR
        " +APPARMOR"
#وإلا
        " -APPARMOR"
#endif

#إذا ENABLE_IMA
        " +IMA"
#وإلا
        " -IMA"
#endif

#إذا ENABLE_IPE
        " +IPE"
#وإلا
        " -IPE"
#endif

#إذا ENABLE_SMACK
        " +SMACK"
#وإلا
        " -SMACK"
#endif

#إذا HAVE_SECCOMP
        " +SECCOMP"
#وإلا
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#إذا HAVE_GNUTLS
        " +GNUTLS"
#وإلا
        " -GNUTLS"
#endif

#إذا HAVE_OPENSSL
        " +OPENSSL"
#وإلا
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#إذا HAVE_ACL
        " +ACL"
#وإلا
        " -ACL"
#endif

#إذا HAVE_BLKID
        " +BLKID"
#وإلا
        " -BLKID"
#endif

#إذا HAVE_LIBCURL
        " +CURL"
#وإلا
        " -CURL"
#endif

#إذا HAVE_ELFUTILS
        " +ELFUTILS"
#وإلا
        " -ELFUTILS"
#endif

#إذا HAVE_LIBFIDO2
        " +FIDO2"
#وإلا
        " -FIDO2"
#endif

#إذا HAVE_LIBIDN2
        " +IDN2"
#وإلا
        " -IDN2"
#endif

#إذا HAVE_KMOD
        " +KMOD"
#وإلا
        " -KMOD"
#endif

#إذا HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#وإلا
        " -LIBCRYPTSETUP"
#endif

#إذا HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#وإلا
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#إذا HAVE_LIBFDISK
        " +LIBFDISK"
#وإلا
        " -LIBFDISK"
#endif

#إذا HAVE_PCRE2
        " +PCRE2"
#وإلا
        " -PCRE2"
#endif

#إذا HAVE_PWQUALITY
        " +PWQUALITY"
#وإلا
        " -PWQUALITY"
#endif

#إذا HAVE_P11KIT
        " +P11KIT"
#وإلا
        " -P11KIT"
#endif

#إذا HAVE_QRENCODE
        " +QRENCODE"
#وإلا
        " -QRENCODE"
#endif

#إذا HAVE_TPM2
        " +TPM2"
#وإلا
        " -TPM2"
#endif

        /* compressors */

#إذا HAVE_BZIP2
        " +BZIP2"
#وإلا
        " -BZIP2"
#endif

#إذا HAVE_LZ4
        " +LZ4"
#وإلا
        " -LZ4"
#endif

#إذا HAVE_XZ
        " +XZ"
#وإلا
        " -XZ"
#endif

#إذا HAVE_ZLIB
        " +ZLIB"
#وإلا
        " -ZLIB"
#endif

#إذا HAVE_ZSTD
        " +ZSTD"
#وإلا
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#إذا BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#وإلا
        " -BPF_FRAMEWORK"
#endif

#إذا HAVE_VMLINUX_H
        " +BTF"
#وإلا
        " -BTF"
#endif

#إذا HAVE_XKBCOMMON
        " +XKBCOMMON"
#وإلا
        " -XKBCOMMON"
#endif

#إذا ENABLE_UTMP
        " +UTMP"
#وإلا
        " -UTMP"
#endif

#إذا HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#وإلا
        " -LIBARCHIVE"
#endif

#إذا EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وإلاإذا EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#وإلا
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
                إذا (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        إرجاع عدم;
                }
                إذا (r == 0)
                        إرجاع TAKE_PTR(ret);

                إذا (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        إرجاع عدم;
                }

                إذا (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                وإلا إذا (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                وإلا
                        q = strextend(&ret, word);
                إذا (!q) {
                        log_oom_warning();
                        إرجاع عدم;
                }
        }
}

صحيح version(فراغ) {
        _cleanup_free_ حرف *b = عدم;

        إذا (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        إرجاع 0;
}

صحيح version_only(فراغ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        إرجاع 0;
}

bool experimental_enabled(فراغ) {
#إذا EXPERIMENTAL == EXPERIMENTAL_DISABLED
        إرجاع false;
#وإلا
        صحيح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        إذا (r >= 0)
                إرجاع r;
        إذا (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        إرجاع EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
