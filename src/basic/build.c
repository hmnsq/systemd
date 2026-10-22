/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "ansi-color.h"
#অন্তর্ভাব "build.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "version.h"

ধ্রুবক অক্ষর* ধ্রুবক systemd_features =

        /* PAM and MAC frameworks */

#যদি HAVE_PAM
        "+PAM"
#অন্যথা
        "-PAM"
#endif

#যদি HAVE_AUDIT
        " +AUDIT"
#অন্যথা
        " -AUDIT"
#endif

#যদি HAVE_SELINUX
        " +SELINUX"
#অন্যথা
        " -SELINUX"
#endif

#যদি HAVE_APPARMOR
        " +APPARMOR"
#অন্যথা
        " -APPARMOR"
#endif

#যদি ENABLE_IMA
        " +IMA"
#অন্যথা
        " -IMA"
#endif

#যদি ENABLE_IPE
        " +IPE"
#অন্যথা
        " -IPE"
#endif

#যদি ENABLE_SMACK
        " +SMACK"
#অন্যথা
        " -SMACK"
#endif

#যদি HAVE_SECCOMP
        " +SECCOMP"
#অন্যথা
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#যদি HAVE_GNUTLS
        " +GNUTLS"
#অন্যথা
        " -GNUTLS"
#endif

#যদি HAVE_OPENSSL
        " +OPENSSL"
#অন্যথা
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#যদি HAVE_ACL
        " +ACL"
#অন্যথা
        " -ACL"
#endif

#যদি HAVE_BLKID
        " +BLKID"
#অন্যথা
        " -BLKID"
#endif

#যদি HAVE_LIBCURL
        " +CURL"
#অন্যথা
        " -CURL"
#endif

#যদি HAVE_ELFUTILS
        " +ELFUTILS"
#অন্যথা
        " -ELFUTILS"
#endif

#যদি HAVE_LIBFIDO2
        " +FIDO2"
#অন্যথা
        " -FIDO2"
#endif

#যদি HAVE_LIBIDN2
        " +IDN2"
#অন্যথা
        " -IDN2"
#endif

#যদি HAVE_KMOD
        " +KMOD"
#অন্যথা
        " -KMOD"
#endif

#যদি HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#অন্যথা
        " -LIBCRYPTSETUP"
#endif

#যদি HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#অন্যথা
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#যদি HAVE_LIBFDISK
        " +LIBFDISK"
#অন্যথা
        " -LIBFDISK"
#endif

#যদি HAVE_PCRE2
        " +PCRE2"
#অন্যথা
        " -PCRE2"
#endif

#যদি HAVE_PWQUALITY
        " +PWQUALITY"
#অন্যথা
        " -PWQUALITY"
#endif

#যদি HAVE_P11KIT
        " +P11KIT"
#অন্যথা
        " -P11KIT"
#endif

#যদি HAVE_QRENCODE
        " +QRENCODE"
#অন্যথা
        " -QRENCODE"
#endif

#যদি HAVE_TPM2
        " +TPM2"
#অন্যথা
        " -TPM2"
#endif

        /* compressors */

#যদি HAVE_BZIP2
        " +BZIP2"
#অন্যথা
        " -BZIP2"
#endif

#যদি HAVE_LZ4
        " +LZ4"
#অন্যথা
        " -LZ4"
#endif

#যদি HAVE_XZ
        " +XZ"
#অন্যথা
        " -XZ"
#endif

#যদি HAVE_ZLIB
        " +ZLIB"
#অন্যথা
        " -ZLIB"
#endif

#যদি HAVE_ZSTD
        " +ZSTD"
#অন্যথা
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#যদি BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#অন্যথা
        " -BPF_FRAMEWORK"
#endif

#যদি HAVE_VMLINUX_H
        " +BTF"
#অন্যথা
        " -BTF"
#endif

#যদি HAVE_XKBCOMMON
        " +XKBCOMMON"
#অন্যথা
        " -XKBCOMMON"
#endif

#যদি ENABLE_UTMP
        " +UTMP"
#অন্যথা
        " -UTMP"
#endif

#যদি HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#অন্যথা
        " -LIBARCHIVE"
#endif

#যদি EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#নতুবা_যদি EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#অন্যথা
        " -EXPERIMENTAL"
#endif
        ;

স্থির অক্ষর* systemd_features_with_color(শূন্য) {
        ধ্রুবক অক্ষর *p = systemd_features;
        _cleanup_free_ অক্ষর *ret = ফাঁকা;
        পূর্ণসংখ্যা r;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *word = ফাঁকা;
                অক্ষর *q;

                r = extract_first_word(&p, &word, ফাঁকা, 0);
                যদি (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ফেরত ফাঁকা;
                }
                যদি (r == 0)
                        ফেরত TAKE_PTR(ret);

                যদি (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ফেরত ফাঁকা;
                }

                যদি (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                অন্যথা যদি (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                অন্যথা
                        q = strextend(&ret, word);
                যদি (!q) {
                        log_oom_warning();
                        ফেরত ফাঁকা;
                }
        }
}

পূর্ণসংখ্যা version(শূন্য) {
        _cleanup_free_ অক্ষর *b = ফাঁকা;

        যদি (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ফেরত 0;
}

পূর্ণসংখ্যা version_only(শূন্য) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ফেরত 0;
}

bool experimental_enabled(শূন্য) {
#যদি EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ফেরত false;
#অন্যথা
        পূর্ণসংখ্যা r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        যদি (r >= 0)
                ফেরত r;
        যদি (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ফেরত EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
