/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "ansi-color.h"
#અન્તર્ભાવ "build.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "version.h"

અચલ અક્ષર* અચલ systemd_features =

        /* PAM and MAC frameworks */

#જો HAVE_PAM
        "+PAM"
#નહીંતર
        "-PAM"
#endif

#જો HAVE_AUDIT
        " +AUDIT"
#નહીંતર
        " -AUDIT"
#endif

#જો HAVE_SELINUX
        " +SELINUX"
#નહીંતર
        " -SELINUX"
#endif

#જો HAVE_APPARMOR
        " +APPARMOR"
#નહીંતર
        " -APPARMOR"
#endif

#જો ENABLE_IMA
        " +IMA"
#નહીંતર
        " -IMA"
#endif

#જો ENABLE_IPE
        " +IPE"
#નહીંતર
        " -IPE"
#endif

#જો ENABLE_SMACK
        " +SMACK"
#નહીંતર
        " -SMACK"
#endif

#જો HAVE_SECCOMP
        " +SECCOMP"
#નહીંતર
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#જો HAVE_GNUTLS
        " +GNUTLS"
#નહીંતર
        " -GNUTLS"
#endif

#જો HAVE_OPENSSL
        " +OPENSSL"
#નહીંતર
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#જો HAVE_ACL
        " +ACL"
#નહીંતર
        " -ACL"
#endif

#જો HAVE_BLKID
        " +BLKID"
#નહીંતર
        " -BLKID"
#endif

#જો HAVE_LIBCURL
        " +CURL"
#નહીંતર
        " -CURL"
#endif

#જો HAVE_ELFUTILS
        " +ELFUTILS"
#નહીંતર
        " -ELFUTILS"
#endif

#જો HAVE_LIBFIDO2
        " +FIDO2"
#નહીંતર
        " -FIDO2"
#endif

#જો HAVE_LIBIDN2
        " +IDN2"
#નહીંતર
        " -IDN2"
#endif

#જો HAVE_KMOD
        " +KMOD"
#નહીંતર
        " -KMOD"
#endif

#જો HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#નહીંતર
        " -LIBCRYPTSETUP"
#endif

#જો HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#નહીંતર
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#જો HAVE_LIBFDISK
        " +LIBFDISK"
#નહીંતર
        " -LIBFDISK"
#endif

#જો HAVE_PCRE2
        " +PCRE2"
#નહીંતર
        " -PCRE2"
#endif

#જો HAVE_PWQUALITY
        " +PWQUALITY"
#નહીંતર
        " -PWQUALITY"
#endif

#જો HAVE_P11KIT
        " +P11KIT"
#નહીંતર
        " -P11KIT"
#endif

#જો HAVE_QRENCODE
        " +QRENCODE"
#નહીંતર
        " -QRENCODE"
#endif

#જો HAVE_TPM2
        " +TPM2"
#નહીંતર
        " -TPM2"
#endif

        /* compressors */

#જો HAVE_BZIP2
        " +BZIP2"
#નહીંતર
        " -BZIP2"
#endif

#જો HAVE_LZ4
        " +LZ4"
#નહીંતર
        " -LZ4"
#endif

#જો HAVE_XZ
        " +XZ"
#નહીંતર
        " -XZ"
#endif

#જો HAVE_ZLIB
        " +ZLIB"
#નહીંતર
        " -ZLIB"
#endif

#જો HAVE_ZSTD
        " +ZSTD"
#નહીંતર
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#જો BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#નહીંતર
        " -BPF_FRAMEWORK"
#endif

#જો HAVE_VMLINUX_H
        " +BTF"
#નહીંતર
        " -BTF"
#endif

#જો HAVE_XKBCOMMON
        " +XKBCOMMON"
#નહીંતર
        " -XKBCOMMON"
#endif

#જો ENABLE_UTMP
        " +UTMP"
#નહીંતર
        " -UTMP"
#endif

#જો HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#નહીંતર
        " -LIBARCHIVE"
#endif

#જો EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#અથવાજો EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#નહીંતર
        " -EXPERIMENTAL"
#endif
        ;

સ્થિર અક્ષર* systemd_features_with_color(શૂન્ય) {
        અચલ અક્ષર *p = systemd_features;
        _cleanup_free_ અક્ષર *ret = ખાલી;
        પૂર્ણાંક r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *word = ખાલી;
                અક્ષર *q;

                r = extract_first_word(&p, &word, ખાલી, 0);
                જો (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        પાછા ખાલી;
                }
                જો (r == 0)
                        પાછા TAKE_PTR(ret);

                જો (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        પાછા ખાલી;
                }

                જો (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                નહીંતર જો (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                નહીંતર
                        q = strextend(&ret, word);
                જો (!q) {
                        log_oom_warning();
                        પાછા ખાલી;
                }
        }
}

પૂર્ણાંક version(શૂન્ય) {
        _cleanup_free_ અક્ષર *b = ખાલી;

        જો (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        પાછા 0;
}

પૂર્ણાંક version_only(શૂન્ય) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        પાછા 0;
}

bool experimental_enabled(શૂન્ય) {
#જો EXPERIMENTAL == EXPERIMENTAL_DISABLED
        પાછા false;
#નહીંતર
        પૂર્ણાંક r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        જો (r >= 0)
                પાછા r;
        જો (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        પાછા EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
