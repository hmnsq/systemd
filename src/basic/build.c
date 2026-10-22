/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "ansi-color.h"
#ଅନ୍ତର୍ଭାଵ "build.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

ଅଚଳ ଅକ୍ଷର* ଅଚଳ systemd_features =

        /* PAM and MAC frameworks */

#ଯଦି HAVE_PAM
        "+PAM"
#ଅନ୍ୟଥା
        "-PAM"
#endif

#ଯଦି HAVE_AUDIT
        " +AUDIT"
#ଅନ୍ୟଥା
        " -AUDIT"
#endif

#ଯଦି HAVE_SELINUX
        " +SELINUX"
#ଅନ୍ୟଥା
        " -SELINUX"
#endif

#ଯଦି HAVE_APPARMOR
        " +APPARMOR"
#ଅନ୍ୟଥା
        " -APPARMOR"
#endif

#ଯଦି ENABLE_IMA
        " +IMA"
#ଅନ୍ୟଥା
        " -IMA"
#endif

#ଯଦି ENABLE_IPE
        " +IPE"
#ଅନ୍ୟଥା
        " -IPE"
#endif

#ଯଦି ENABLE_SMACK
        " +SMACK"
#ଅନ୍ୟଥା
        " -SMACK"
#endif

#ଯଦି HAVE_SECCOMP
        " +SECCOMP"
#ଅନ୍ୟଥା
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ଯଦି HAVE_GNUTLS
        " +GNUTLS"
#ଅନ୍ୟଥା
        " -GNUTLS"
#endif

#ଯଦି HAVE_OPENSSL
        " +OPENSSL"
#ଅନ୍ୟଥା
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ଯଦି HAVE_ACL
        " +ACL"
#ଅନ୍ୟଥା
        " -ACL"
#endif

#ଯଦି HAVE_BLKID
        " +BLKID"
#ଅନ୍ୟଥା
        " -BLKID"
#endif

#ଯଦି HAVE_LIBCURL
        " +CURL"
#ଅନ୍ୟଥା
        " -CURL"
#endif

#ଯଦି HAVE_ELFUTILS
        " +ELFUTILS"
#ଅନ୍ୟଥା
        " -ELFUTILS"
#endif

#ଯଦି HAVE_LIBFIDO2
        " +FIDO2"
#ଅନ୍ୟଥା
        " -FIDO2"
#endif

#ଯଦି HAVE_LIBIDN2
        " +IDN2"
#ଅନ୍ୟଥା
        " -IDN2"
#endif

#ଯଦି HAVE_KMOD
        " +KMOD"
#ଅନ୍ୟଥା
        " -KMOD"
#endif

#ଯଦି HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ଅନ୍ୟଥା
        " -LIBCRYPTSETUP"
#endif

#ଯଦି HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ଅନ୍ୟଥା
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ଯଦି HAVE_LIBFDISK
        " +LIBFDISK"
#ଅନ୍ୟଥା
        " -LIBFDISK"
#endif

#ଯଦି HAVE_PCRE2
        " +PCRE2"
#ଅନ୍ୟଥା
        " -PCRE2"
#endif

#ଯଦି HAVE_PWQUALITY
        " +PWQUALITY"
#ଅନ୍ୟଥା
        " -PWQUALITY"
#endif

#ଯଦି HAVE_P11KIT
        " +P11KIT"
#ଅନ୍ୟଥା
        " -P11KIT"
#endif

#ଯଦି HAVE_QRENCODE
        " +QRENCODE"
#ଅନ୍ୟଥା
        " -QRENCODE"
#endif

#ଯଦି HAVE_TPM2
        " +TPM2"
#ଅନ୍ୟଥା
        " -TPM2"
#endif

        /* compressors */

#ଯଦି HAVE_BZIP2
        " +BZIP2"
#ଅନ୍ୟଥା
        " -BZIP2"
#endif

#ଯଦି HAVE_LZ4
        " +LZ4"
#ଅନ୍ୟଥା
        " -LZ4"
#endif

#ଯଦି HAVE_XZ
        " +XZ"
#ଅନ୍ୟଥା
        " -XZ"
#endif

#ଯଦି HAVE_ZLIB
        " +ZLIB"
#ଅନ୍ୟଥା
        " -ZLIB"
#endif

#ଯଦି HAVE_ZSTD
        " +ZSTD"
#ଅନ୍ୟଥା
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ଯଦି BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ଅନ୍ୟଥା
        " -BPF_FRAMEWORK"
#endif

#ଯଦି HAVE_VMLINUX_H
        " +BTF"
#ଅନ୍ୟଥା
        " -BTF"
#endif

#ଯଦି HAVE_XKBCOMMON
        " +XKBCOMMON"
#ଅନ୍ୟଥା
        " -XKBCOMMON"
#endif

#ଯଦି ENABLE_UTMP
        " +UTMP"
#ଅନ୍ୟଥା
        " -UTMP"
#endif

#ଯଦି HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ଅନ୍ୟଥା
        " -LIBARCHIVE"
#endif

#ଯଦି EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#ନଚେତଯଦି EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ଅନ୍ୟଥା
        " -EXPERIMENTAL"
#endif
        ;

ସ୍ଥିର ଅକ୍ଷର* systemd_features_with_color(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *p = systemd_features;
        _cleanup_free_ ଅକ୍ଷର *ret = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *word = ଖାଲି;
                ଅକ୍ଷର *q;

                r = extract_first_word(&p, &word, ଖାଲି, 0);
                ଯଦି (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ଫେରନ୍ତୁ ଖାଲି;
                }
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ TAKE_PTR(ret);

                ଯଦି (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ଫେରନ୍ତୁ ଖାଲି;
                }

                ଯଦି (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ଅନ୍ୟଥା ଯଦି (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ଅନ୍ୟଥା
                        q = strextend(&ret, word);
                ଯଦି (!q) {
                        log_oom_warning();
                        ଫେରନ୍ତୁ ଖାଲି;
                }
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା version(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *b = ଖାଲି;

        ଯଦି (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା version_only(ଶୂନ୍ୟ) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ଫେରନ୍ତୁ 0;
}

bool experimental_enabled(ଶୂନ୍ୟ) {
#ଯଦି EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ଫେରନ୍ତୁ false;
#ଅନ୍ୟଥା
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ଫେରନ୍ତୁ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
