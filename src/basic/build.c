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

#𐡄𐡍 HAVE_PAM
        "+PAM"
#وإلا
        "-PAM"
#endif

#𐡄𐡍 HAVE_AUDIT
        " +AUDIT"
#وإلا
        " -AUDIT"
#endif

#𐡄𐡍 HAVE_SELINUX
        " +SELINUX"
#وإلا
        " -SELINUX"
#endif

#𐡄𐡍 HAVE_APPARMOR
        " +APPARMOR"
#وإلا
        " -APPARMOR"
#endif

#𐡄𐡍 ENABLE_IMA
        " +IMA"
#وإلا
        " -IMA"
#endif

#𐡄𐡍 ENABLE_IPE
        " +IPE"
#وإلا
        " -IPE"
#endif

#𐡄𐡍 ENABLE_SMACK
        " +SMACK"
#وإلا
        " -SMACK"
#endif

#𐡄𐡍 HAVE_SECCOMP
        " +SECCOMP"
#وإلا
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#𐡄𐡍 HAVE_GNUTLS
        " +GNUTLS"
#وإلا
        " -GNUTLS"
#endif

#𐡄𐡍 HAVE_OPENSSL
        " +OPENSSL"
#وإلا
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#𐡄𐡍 HAVE_ACL
        " +ACL"
#وإلا
        " -ACL"
#endif

#𐡄𐡍 HAVE_BLKID
        " +BLKID"
#وإلا
        " -BLKID"
#endif

#𐡄𐡍 HAVE_LIBCURL
        " +CURL"
#وإلا
        " -CURL"
#endif

#𐡄𐡍 HAVE_ELFUTILS
        " +ELFUTILS"
#وإلا
        " -ELFUTILS"
#endif

#𐡄𐡍 HAVE_LIBFIDO2
        " +FIDO2"
#وإلا
        " -FIDO2"
#endif

#𐡄𐡍 HAVE_LIBIDN2
        " +IDN2"
#وإلا
        " -IDN2"
#endif

#𐡄𐡍 HAVE_KMOD
        " +KMOD"
#وإلا
        " -KMOD"
#endif

#𐡄𐡍 HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#وإلا
        " -LIBCRYPTSETUP"
#endif

#𐡄𐡍 HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#وإلا
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#𐡄𐡍 HAVE_LIBFDISK
        " +LIBFDISK"
#وإلا
        " -LIBFDISK"
#endif

#𐡄𐡍 HAVE_PCRE2
        " +PCRE2"
#وإلا
        " -PCRE2"
#endif

#𐡄𐡍 HAVE_PWQUALITY
        " +PWQUALITY"
#وإلا
        " -PWQUALITY"
#endif

#𐡄𐡍 HAVE_P11KIT
        " +P11KIT"
#وإلا
        " -P11KIT"
#endif

#𐡄𐡍 HAVE_QRENCODE
        " +QRENCODE"
#وإلا
        " -QRENCODE"
#endif

#𐡄𐡍 HAVE_TPM2
        " +TPM2"
#وإلا
        " -TPM2"
#endif

        /* compressors */

#𐡄𐡍 HAVE_BZIP2
        " +BZIP2"
#وإلا
        " -BZIP2"
#endif

#𐡄𐡍 HAVE_LZ4
        " +LZ4"
#وإلا
        " -LZ4"
#endif

#𐡄𐡍 HAVE_XZ
        " +XZ"
#وإلا
        " -XZ"
#endif

#𐡄𐡍 HAVE_ZLIB
        " +ZLIB"
#وإلا
        " -ZLIB"
#endif

#𐡄𐡍 HAVE_ZSTD
        " +ZSTD"
#وإلا
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#𐡄𐡍 BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#وإلا
        " -BPF_FRAMEWORK"
#endif

#𐡄𐡍 HAVE_VMLINUX_H
        " +BTF"
#وإلا
        " -BTF"
#endif

#𐡄𐡍 HAVE_XKBCOMMON
        " +XKBCOMMON"
#وإلا
        " -XKBCOMMON"
#endif

#𐡄𐡍 ENABLE_UTMP
        " +UTMP"
#وإلا
        " -UTMP"
#endif

#𐡄𐡍 HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#وإلا
        " -LIBARCHIVE"
#endif

#𐡄𐡍 EXPERIMENTAL == EXPERIMENTAL_ENABLED
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
                𐡄𐡍 (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        أرجع عدم;
                }
                𐡄𐡍 (r == 0)
                        أرجع TAKE_PTR(ret);

                𐡄𐡍 (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        أرجع عدم;
                }

                𐡄𐡍 (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                وإلا 𐡄𐡍 (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                وإلا
                        q = strextend(&ret, word);
                𐡄𐡍 (!q) {
                        log_oom_warning();
                        أرجع عدم;
                }
        }
}

صحيح version(فراغ) {
        _cleanup_free_ حرف *b = عدم;

        𐡄𐡍 (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        أرجع 0;
}

صحيح version_only(فراغ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        أرجع 0;
}

bool experimental_enabled(فراغ) {
#𐡄𐡍 EXPERIMENTAL == EXPERIMENTAL_DISABLED
        أرجع false;
#وإلا
        صحيح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        𐡄𐡍 (r >= 0)
                أرجع r;
        𐡄𐡍 (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        أرجع EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
