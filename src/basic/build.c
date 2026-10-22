/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-color.h"
#अन्तर्भाव "build.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "version.h"

ध्रुव वर्ण* ध्रुव systemd_features =

        /* PAM and MAC frameworks */

#सचे HAVE_PAM
        "+PAM"
#अञ्ञथा
        "-PAM"
#endif

#सचे HAVE_AUDIT
        " +AUDIT"
#अञ्ञथा
        " -AUDIT"
#endif

#सचे HAVE_SELINUX
        " +SELINUX"
#अञ्ञथा
        " -SELINUX"
#endif

#सचे HAVE_APPARMOR
        " +APPARMOR"
#अञ्ञथा
        " -APPARMOR"
#endif

#सचे ENABLE_IMA
        " +IMA"
#अञ्ञथा
        " -IMA"
#endif

#सचे ENABLE_IPE
        " +IPE"
#अञ्ञथा
        " -IPE"
#endif

#सचे ENABLE_SMACK
        " +SMACK"
#अञ्ञथा
        " -SMACK"
#endif

#सचे HAVE_SECCOMP
        " +SECCOMP"
#अञ्ञथा
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#सचे HAVE_GNUTLS
        " +GNUTLS"
#अञ्ञथा
        " -GNUTLS"
#endif

#सचे HAVE_OPENSSL
        " +OPENSSL"
#अञ्ञथा
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#सचे HAVE_ACL
        " +ACL"
#अञ्ञथा
        " -ACL"
#endif

#सचे HAVE_BLKID
        " +BLKID"
#अञ्ञथा
        " -BLKID"
#endif

#सचे HAVE_LIBCURL
        " +CURL"
#अञ्ञथा
        " -CURL"
#endif

#सचे HAVE_ELFUTILS
        " +ELFUTILS"
#अञ्ञथा
        " -ELFUTILS"
#endif

#सचे HAVE_LIBFIDO2
        " +FIDO2"
#अञ्ञथा
        " -FIDO2"
#endif

#सचे HAVE_LIBIDN2
        " +IDN2"
#अञ्ञथा
        " -IDN2"
#endif

#सचे HAVE_KMOD
        " +KMOD"
#अञ्ञथा
        " -KMOD"
#endif

#सचे HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#अञ्ञथा
        " -LIBCRYPTSETUP"
#endif

#सचे HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#अञ्ञथा
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#सचे HAVE_LIBFDISK
        " +LIBFDISK"
#अञ्ञथा
        " -LIBFDISK"
#endif

#सचे HAVE_PCRE2
        " +PCRE2"
#अञ्ञथा
        " -PCRE2"
#endif

#सचे HAVE_PWQUALITY
        " +PWQUALITY"
#अञ्ञथा
        " -PWQUALITY"
#endif

#सचे HAVE_P11KIT
        " +P11KIT"
#अञ्ञथा
        " -P11KIT"
#endif

#सचे HAVE_QRENCODE
        " +QRENCODE"
#अञ्ञथा
        " -QRENCODE"
#endif

#सचे HAVE_TPM2
        " +TPM2"
#अञ्ञथा
        " -TPM2"
#endif

        /* compressors */

#सचे HAVE_BZIP2
        " +BZIP2"
#अञ्ञथा
        " -BZIP2"
#endif

#सचे HAVE_LZ4
        " +LZ4"
#अञ्ञथा
        " -LZ4"
#endif

#सचे HAVE_XZ
        " +XZ"
#अञ्ञथा
        " -XZ"
#endif

#सचे HAVE_ZLIB
        " +ZLIB"
#अञ्ञथा
        " -ZLIB"
#endif

#सचे HAVE_ZSTD
        " +ZSTD"
#अञ्ञथा
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#सचे BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#अञ्ञथा
        " -BPF_FRAMEWORK"
#endif

#सचे HAVE_VMLINUX_H
        " +BTF"
#अञ्ञथा
        " -BTF"
#endif

#सचे HAVE_XKBCOMMON
        " +XKBCOMMON"
#अञ्ञथा
        " -XKBCOMMON"
#endif

#सचे ENABLE_UTMP
        " +UTMP"
#अञ्ञथा
        " -UTMP"
#endif

#सचे HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#अञ्ञथा
        " -LIBARCHIVE"
#endif

#सचे EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#नोचेद्यदि EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#अञ्ञथा
        " -EXPERIMENTAL"
#endif
        ;

स्थिर वर्ण* systemd_features_with_color(शून्य) {
        ध्रुव वर्ण *p = systemd_features;
        _cleanup_free_ वर्ण *ret = रिक्त;
        पूर्णाङ्क r;

        अत्थाय (;;) {
                _cleanup_free_ वर्ण *word = रिक्त;
                वर्ण *q;

                r = extract_first_word(&p, &word, रिक्त, 0);
                सचे (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        पटिनिवत्त रिक्त;
                }
                सचे (r == 0)
                        पटिनिवत्त TAKE_PTR(ret);

                सचे (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        पटिनिवत्त रिक्त;
                }

                सचे (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                अञ्ञथा सचे (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                अञ्ञथा
                        q = strextend(&ret, word);
                सचे (!q) {
                        log_oom_warning();
                        पटिनिवत्त रिक्त;
                }
        }
}

पूर्णाङ्क version(शून्य) {
        _cleanup_free_ वर्ण *b = रिक्त;

        सचे (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        पटिनिवत्त 0;
}

पूर्णाङ्क version_only(शून्य) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        पटिनिवत्त 0;
}

bool experimental_enabled(शून्य) {
#सचे EXPERIMENTAL == EXPERIMENTAL_DISABLED
        पटिनिवत्त false;
#अञ्ञथा
        पूर्णाङ्क r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        सचे (r >= 0)
                पटिनिवत्त r;
        सचे (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        पटिनिवत्त EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
