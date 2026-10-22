/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "ansi-color.h"
#അന്തര്ഭാവ "build.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "version.h"

സ്ഥിരം അക്ഷരം* സ്ഥിരം systemd_features =

        /* PAM and MAC frameworks */

#എങ്കിൽ HAVE_PAM
        "+PAM"
#അല്ലെങ്കിൽ
        "-PAM"
#endif

#എങ്കിൽ HAVE_AUDIT
        " +AUDIT"
#അല്ലെങ്കിൽ
        " -AUDIT"
#endif

#എങ്കിൽ HAVE_SELINUX
        " +SELINUX"
#അല്ലെങ്കിൽ
        " -SELINUX"
#endif

#എങ്കിൽ HAVE_APPARMOR
        " +APPARMOR"
#അല്ലെങ്കിൽ
        " -APPARMOR"
#endif

#എങ്കിൽ ENABLE_IMA
        " +IMA"
#അല്ലെങ്കിൽ
        " -IMA"
#endif

#എങ്കിൽ ENABLE_IPE
        " +IPE"
#അല്ലെങ്കിൽ
        " -IPE"
#endif

#എങ്കിൽ ENABLE_SMACK
        " +SMACK"
#അല്ലെങ്കിൽ
        " -SMACK"
#endif

#എങ്കിൽ HAVE_SECCOMP
        " +SECCOMP"
#അല്ലെങ്കിൽ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#എങ്കിൽ HAVE_GNUTLS
        " +GNUTLS"
#അല്ലെങ്കിൽ
        " -GNUTLS"
#endif

#എങ്കിൽ HAVE_OPENSSL
        " +OPENSSL"
#അല്ലെങ്കിൽ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#എങ്കിൽ HAVE_ACL
        " +ACL"
#അല്ലെങ്കിൽ
        " -ACL"
#endif

#എങ്കിൽ HAVE_BLKID
        " +BLKID"
#അല്ലെങ്കിൽ
        " -BLKID"
#endif

#എങ്കിൽ HAVE_LIBCURL
        " +CURL"
#അല്ലെങ്കിൽ
        " -CURL"
#endif

#എങ്കിൽ HAVE_ELFUTILS
        " +ELFUTILS"
#അല്ലെങ്കിൽ
        " -ELFUTILS"
#endif

#എങ്കിൽ HAVE_LIBFIDO2
        " +FIDO2"
#അല്ലെങ്കിൽ
        " -FIDO2"
#endif

#എങ്കിൽ HAVE_LIBIDN2
        " +IDN2"
#അല്ലെങ്കിൽ
        " -IDN2"
#endif

#എങ്കിൽ HAVE_KMOD
        " +KMOD"
#അല്ലെങ്കിൽ
        " -KMOD"
#endif

#എങ്കിൽ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#അല്ലെങ്കിൽ
        " -LIBCRYPTSETUP"
#endif

#എങ്കിൽ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#അല്ലെങ്കിൽ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#എങ്കിൽ HAVE_LIBFDISK
        " +LIBFDISK"
#അല്ലെങ്കിൽ
        " -LIBFDISK"
#endif

#എങ്കിൽ HAVE_PCRE2
        " +PCRE2"
#അല്ലെങ്കിൽ
        " -PCRE2"
#endif

#എങ്കിൽ HAVE_PWQUALITY
        " +PWQUALITY"
#അല്ലെങ്കിൽ
        " -PWQUALITY"
#endif

#എങ്കിൽ HAVE_P11KIT
        " +P11KIT"
#അല്ലെങ്കിൽ
        " -P11KIT"
#endif

#എങ്കിൽ HAVE_QRENCODE
        " +QRENCODE"
#അല്ലെങ്കിൽ
        " -QRENCODE"
#endif

#എങ്കിൽ HAVE_TPM2
        " +TPM2"
#അല്ലെങ്കിൽ
        " -TPM2"
#endif

        /* compressors */

#എങ്കിൽ HAVE_BZIP2
        " +BZIP2"
#അല്ലെങ്കിൽ
        " -BZIP2"
#endif

#എങ്കിൽ HAVE_LZ4
        " +LZ4"
#അല്ലെങ്കിൽ
        " -LZ4"
#endif

#എങ്കിൽ HAVE_XZ
        " +XZ"
#അല്ലെങ്കിൽ
        " -XZ"
#endif

#എങ്കിൽ HAVE_ZLIB
        " +ZLIB"
#അല്ലെങ്കിൽ
        " -ZLIB"
#endif

#എങ്കിൽ HAVE_ZSTD
        " +ZSTD"
#അല്ലെങ്കിൽ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#എങ്കിൽ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#അല്ലെങ്കിൽ
        " -BPF_FRAMEWORK"
#endif

#എങ്കിൽ HAVE_VMLINUX_H
        " +BTF"
#അല്ലെങ്കിൽ
        " -BTF"
#endif

#എങ്കിൽ HAVE_XKBCOMMON
        " +XKBCOMMON"
#അല്ലെങ്കിൽ
        " -XKBCOMMON"
#endif

#എങ്കിൽ ENABLE_UTMP
        " +UTMP"
#അല്ലെങ്കിൽ
        " -UTMP"
#endif

#എങ്കിൽ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#അല്ലെങ്കിൽ
        " -LIBARCHIVE"
#endif

#എങ്കിൽ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#അല്ലെങ്കിലെങ്കിൽ EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#അല്ലെങ്കിൽ
        " -EXPERIMENTAL"
#endif
        ;

സ്ഥിതം അക്ഷരം* systemd_features_with_color(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *p = systemd_features;
        _cleanup_free_ അക്ഷരം *ret = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *word = ശൂന്യ;
                അക്ഷരം *q;

                r = extract_first_word(&p, &word, ശൂന്യ, 0);
                എങ്കിൽ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        തിരികെ ശൂന്യ;
                }
                എങ്കിൽ (r == 0)
                        തിരികെ TAKE_PTR(ret);

                എങ്കിൽ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        തിരികെ ശൂന്യ;
                }

                എങ്കിൽ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                അല്ലെങ്കിൽ എങ്കിൽ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                അല്ലെങ്കിൽ
                        q = strextend(&ret, word);
                എങ്കിൽ (!q) {
                        log_oom_warning();
                        തിരികെ ശൂന്യ;
                }
        }
}

പൂർണ്ണസംഖ്യ version(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *b = ശൂന്യ;

        എങ്കിൽ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ version_only(ശൂന്യം) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        തിരികെ 0;
}

bool experimental_enabled(ശൂന്യം) {
#എങ്കിൽ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        തിരികെ false;
#അല്ലെങ്കിൽ
        പൂർണ്ണസംഖ്യ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        എങ്കിൽ (r >= 0)
                തിരികെ r;
        എങ്കിൽ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        തിരികെ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
