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

#אם HAVE_PAM
        "+PAM"
#אחרת
        "-PAM"
#endif

#אם HAVE_AUDIT
        " +AUDIT"
#אחרת
        " -AUDIT"
#endif

#אם HAVE_SELINUX
        " +SELINUX"
#אחרת
        " -SELINUX"
#endif

#אם HAVE_APPARMOR
        " +APPARMOR"
#אחרת
        " -APPARMOR"
#endif

#אם ENABLE_IMA
        " +IMA"
#אחרת
        " -IMA"
#endif

#אם ENABLE_IPE
        " +IPE"
#אחרת
        " -IPE"
#endif

#אם ENABLE_SMACK
        " +SMACK"
#אחרת
        " -SMACK"
#endif

#אם HAVE_SECCOMP
        " +SECCOMP"
#אחרת
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#אם HAVE_GNUTLS
        " +GNUTLS"
#אחרת
        " -GNUTLS"
#endif

#אם HAVE_OPENSSL
        " +OPENSSL"
#אחרת
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#אם HAVE_ACL
        " +ACL"
#אחרת
        " -ACL"
#endif

#אם HAVE_BLKID
        " +BLKID"
#אחרת
        " -BLKID"
#endif

#אם HAVE_LIBCURL
        " +CURL"
#אחרת
        " -CURL"
#endif

#אם HAVE_ELFUTILS
        " +ELFUTILS"
#אחרת
        " -ELFUTILS"
#endif

#אם HAVE_LIBFIDO2
        " +FIDO2"
#אחרת
        " -FIDO2"
#endif

#אם HAVE_LIBIDN2
        " +IDN2"
#אחרת
        " -IDN2"
#endif

#אם HAVE_KMOD
        " +KMOD"
#אחרת
        " -KMOD"
#endif

#אם HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#אחרת
        " -LIBCRYPTSETUP"
#endif

#אם HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#אחרת
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#אם HAVE_LIBFDISK
        " +LIBFDISK"
#אחרת
        " -LIBFDISK"
#endif

#אם HAVE_PCRE2
        " +PCRE2"
#אחרת
        " -PCRE2"
#endif

#אם HAVE_PWQUALITY
        " +PWQUALITY"
#אחרת
        " -PWQUALITY"
#endif

#אם HAVE_P11KIT
        " +P11KIT"
#אחרת
        " -P11KIT"
#endif

#אם HAVE_QRENCODE
        " +QRENCODE"
#אחרת
        " -QRENCODE"
#endif

#אם HAVE_TPM2
        " +TPM2"
#אחרת
        " -TPM2"
#endif

        /* compressors */

#אם HAVE_BZIP2
        " +BZIP2"
#אחרת
        " -BZIP2"
#endif

#אם HAVE_LZ4
        " +LZ4"
#אחרת
        " -LZ4"
#endif

#אם HAVE_XZ
        " +XZ"
#אחרת
        " -XZ"
#endif

#אם HAVE_ZLIB
        " +ZLIB"
#אחרת
        " -ZLIB"
#endif

#אם HAVE_ZSTD
        " +ZSTD"
#אחרת
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#אם BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#אחרת
        " -BPF_FRAMEWORK"
#endif

#אם HAVE_VMLINUX_H
        " +BTF"
#אחרת
        " -BTF"
#endif

#אם HAVE_XKBCOMMON
        " +XKBCOMMON"
#אחרת
        " -XKBCOMMON"
#endif

#אם ENABLE_UTMP
        " +UTMP"
#אחרת
        " -UTMP"
#endif

#אם HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#אחרת
        " -LIBARCHIVE"
#endif

#אם EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وإلاإذا EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#אחרת
        " -EXPERIMENTAL"
#endif
        ;

ساكن حرف* systemd_features_with_color(فراغ) {
        ثابت حرف *p = systemd_features;
        _cleanup_free_ حرف *ret = عدم;
        صحيح r;

        עבור (;;) {
                _cleanup_free_ حرف *word = عدم;
                حرف *q;

                r = extract_first_word(&p, &word, عدم, 0);
                אם (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        החזר عدم;
                }
                אם (r == 0)
                        החזר TAKE_PTR(ret);

                אם (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        החזר عدم;
                }

                אם (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                אחרת אם (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                אחרת
                        q = strextend(&ret, word);
                אם (!q) {
                        log_oom_warning();
                        החזר عدم;
                }
        }
}

صحيح version(فراغ) {
        _cleanup_free_ حرف *b = عدم;

        אם (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        החזר 0;
}

صحيح version_only(فراغ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        החזר 0;
}

bool experimental_enabled(فراغ) {
#אם EXPERIMENTAL == EXPERIMENTAL_DISABLED
        החזר false;
#אחרת
        صحيح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        אם (r >= 0)
                החזר r;
        אם (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        החזר EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
