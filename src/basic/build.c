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

#यदि HAVE_PAM
        "+PAM"
#अन्यथा
        "-PAM"
#endif

#यदि HAVE_AUDIT
        " +AUDIT"
#अन्यथा
        " -AUDIT"
#endif

#यदि HAVE_SELINUX
        " +SELINUX"
#अन्यथा
        " -SELINUX"
#endif

#यदि HAVE_APPARMOR
        " +APPARMOR"
#अन्यथा
        " -APPARMOR"
#endif

#यदि ENABLE_IMA
        " +IMA"
#अन्यथा
        " -IMA"
#endif

#यदि ENABLE_IPE
        " +IPE"
#अन्यथा
        " -IPE"
#endif

#यदि ENABLE_SMACK
        " +SMACK"
#अन्यथा
        " -SMACK"
#endif

#यदि HAVE_SECCOMP
        " +SECCOMP"
#अन्यथा
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#यदि HAVE_GNUTLS
        " +GNUTLS"
#अन्यथा
        " -GNUTLS"
#endif

#यदि HAVE_OPENSSL
        " +OPENSSL"
#अन्यथा
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#यदि HAVE_ACL
        " +ACL"
#अन्यथा
        " -ACL"
#endif

#यदि HAVE_BLKID
        " +BLKID"
#अन्यथा
        " -BLKID"
#endif

#यदि HAVE_LIBCURL
        " +CURL"
#अन्यथा
        " -CURL"
#endif

#यदि HAVE_ELFUTILS
        " +ELFUTILS"
#अन्यथा
        " -ELFUTILS"
#endif

#यदि HAVE_LIBFIDO2
        " +FIDO2"
#अन्यथा
        " -FIDO2"
#endif

#यदि HAVE_LIBIDN2
        " +IDN2"
#अन्यथा
        " -IDN2"
#endif

#यदि HAVE_KMOD
        " +KMOD"
#अन्यथा
        " -KMOD"
#endif

#यदि HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#अन्यथा
        " -LIBCRYPTSETUP"
#endif

#यदि HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#अन्यथा
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#यदि HAVE_LIBFDISK
        " +LIBFDISK"
#अन्यथा
        " -LIBFDISK"
#endif

#यदि HAVE_PCRE2
        " +PCRE2"
#अन्यथा
        " -PCRE2"
#endif

#यदि HAVE_PWQUALITY
        " +PWQUALITY"
#अन्यथा
        " -PWQUALITY"
#endif

#यदि HAVE_P11KIT
        " +P11KIT"
#अन्यथा
        " -P11KIT"
#endif

#यदि HAVE_QRENCODE
        " +QRENCODE"
#अन्यथा
        " -QRENCODE"
#endif

#यदि HAVE_TPM2
        " +TPM2"
#अन्यथा
        " -TPM2"
#endif

        /* compressors */

#यदि HAVE_BZIP2
        " +BZIP2"
#अन्यथा
        " -BZIP2"
#endif

#यदि HAVE_LZ4
        " +LZ4"
#अन्यथा
        " -LZ4"
#endif

#यदि HAVE_XZ
        " +XZ"
#अन्यथा
        " -XZ"
#endif

#यदि HAVE_ZLIB
        " +ZLIB"
#अन्यथा
        " -ZLIB"
#endif

#यदि HAVE_ZSTD
        " +ZSTD"
#अन्यथा
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#यदि BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#अन्यथा
        " -BPF_FRAMEWORK"
#endif

#यदि HAVE_VMLINUX_H
        " +BTF"
#अन्यथा
        " -BTF"
#endif

#यदि HAVE_XKBCOMMON
        " +XKBCOMMON"
#अन्यथा
        " -XKBCOMMON"
#endif

#यदि ENABLE_UTMP
        " +UTMP"
#अन्यथा
        " -UTMP"
#endif

#यदि HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#अन्यथा
        " -LIBARCHIVE"
#endif

#यदि EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#नोचेद्यदि EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#अन्यथा
        " -EXPERIMENTAL"
#endif
        ;

स्थिर वर्ण* systemd_features_with_color(शून्य) {
        ध्रुव वर्ण *p = systemd_features;
        _cleanup_free_ वर्ण *ret = रिक्त;
        पूर्णाङ्क r;

        प्रति (;;) {
                _cleanup_free_ वर्ण *word = रिक्त;
                वर्ण *q;

                r = extract_first_word(&p, &word, रिक्त, 0);
                यदि (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        प्रत्यागम रिक्त;
                }
                यदि (r == 0)
                        प्रत्यागम TAKE_PTR(ret);

                यदि (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        प्रत्यागम रिक्त;
                }

                यदि (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                अन्यथा यदि (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                अन्यथा
                        q = strextend(&ret, word);
                यदि (!q) {
                        log_oom_warning();
                        प्रत्यागम रिक्त;
                }
        }
}

पूर्णाङ्क version(शून्य) {
        _cleanup_free_ वर्ण *b = रिक्त;

        यदि (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        प्रत्यागम 0;
}

पूर्णाङ्क version_only(शून्य) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        प्रत्यागम 0;
}

bool experimental_enabled(शून्य) {
#यदि EXPERIMENTAL == EXPERIMENTAL_DISABLED
        प्रत्यागम false;
#अन्यथा
        पूर्णाङ्क r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        यदि (r >= 0)
                प्रत्यागम r;
        यदि (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        प्रत्यागम EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
