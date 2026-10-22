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

#जइ HAVE_PAM
        "+PAM"
#अण्णहा
        "-PAM"
#endif

#जइ HAVE_AUDIT
        " +AUDIT"
#अण्णहा
        " -AUDIT"
#endif

#जइ HAVE_SELINUX
        " +SELINUX"
#अण्णहा
        " -SELINUX"
#endif

#जइ HAVE_APPARMOR
        " +APPARMOR"
#अण्णहा
        " -APPARMOR"
#endif

#जइ ENABLE_IMA
        " +IMA"
#अण्णहा
        " -IMA"
#endif

#जइ ENABLE_IPE
        " +IPE"
#अण्णहा
        " -IPE"
#endif

#जइ ENABLE_SMACK
        " +SMACK"
#अण्णहा
        " -SMACK"
#endif

#जइ HAVE_SECCOMP
        " +SECCOMP"
#अण्णहा
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#जइ HAVE_GNUTLS
        " +GNUTLS"
#अण्णहा
        " -GNUTLS"
#endif

#जइ HAVE_OPENSSL
        " +OPENSSL"
#अण्णहा
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#जइ HAVE_ACL
        " +ACL"
#अण्णहा
        " -ACL"
#endif

#जइ HAVE_BLKID
        " +BLKID"
#अण्णहा
        " -BLKID"
#endif

#जइ HAVE_LIBCURL
        " +CURL"
#अण्णहा
        " -CURL"
#endif

#जइ HAVE_ELFUTILS
        " +ELFUTILS"
#अण्णहा
        " -ELFUTILS"
#endif

#जइ HAVE_LIBFIDO2
        " +FIDO2"
#अण्णहा
        " -FIDO2"
#endif

#जइ HAVE_LIBIDN2
        " +IDN2"
#अण्णहा
        " -IDN2"
#endif

#जइ HAVE_KMOD
        " +KMOD"
#अण्णहा
        " -KMOD"
#endif

#जइ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#अण्णहा
        " -LIBCRYPTSETUP"
#endif

#जइ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#अण्णहा
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#जइ HAVE_LIBFDISK
        " +LIBFDISK"
#अण्णहा
        " -LIBFDISK"
#endif

#जइ HAVE_PCRE2
        " +PCRE2"
#अण्णहा
        " -PCRE2"
#endif

#जइ HAVE_PWQUALITY
        " +PWQUALITY"
#अण्णहा
        " -PWQUALITY"
#endif

#जइ HAVE_P11KIT
        " +P11KIT"
#अण्णहा
        " -P11KIT"
#endif

#जइ HAVE_QRENCODE
        " +QRENCODE"
#अण्णहा
        " -QRENCODE"
#endif

#जइ HAVE_TPM2
        " +TPM2"
#अण्णहा
        " -TPM2"
#endif

        /* compressors */

#जइ HAVE_BZIP2
        " +BZIP2"
#अण्णहा
        " -BZIP2"
#endif

#जइ HAVE_LZ4
        " +LZ4"
#अण्णहा
        " -LZ4"
#endif

#जइ HAVE_XZ
        " +XZ"
#अण्णहा
        " -XZ"
#endif

#जइ HAVE_ZLIB
        " +ZLIB"
#अण्णहा
        " -ZLIB"
#endif

#जइ HAVE_ZSTD
        " +ZSTD"
#अण्णहा
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#जइ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#अण्णहा
        " -BPF_FRAMEWORK"
#endif

#जइ HAVE_VMLINUX_H
        " +BTF"
#अण्णहा
        " -BTF"
#endif

#जइ HAVE_XKBCOMMON
        " +XKBCOMMON"
#अण्णहा
        " -XKBCOMMON"
#endif

#जइ ENABLE_UTMP
        " +UTMP"
#अण्णहा
        " -UTMP"
#endif

#जइ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#अण्णहा
        " -LIBARCHIVE"
#endif

#जइ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#नोचेद्यदि EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#अण्णहा
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
                जइ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        प्रत्यागम रिक्त;
                }
                जइ (r == 0)
                        प्रत्यागम TAKE_PTR(ret);

                जइ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        प्रत्यागम रिक्त;
                }

                जइ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                अण्णहा जइ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                अण्णहा
                        q = strextend(&ret, word);
                जइ (!q) {
                        log_oom_warning();
                        प्रत्यागम रिक्त;
                }
        }
}

पूर्णाङ्क version(शून्य) {
        _cleanup_free_ वर्ण *b = रिक्त;

        जइ (colors_enabled())
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
#जइ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        प्रत्यागम false;
#अण्णहा
        पूर्णाङ्क r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        जइ (r >= 0)
                प्रत्यागम r;
        जइ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        प्रत्यागम EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
