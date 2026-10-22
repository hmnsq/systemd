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

स्थिरांक अक्षर* स्थिरांक systemd_features =

        /* PAM and MAC frameworks */

#जर HAVE_PAM
        "+PAM"
#अन्यथा
        "-PAM"
#endif

#जर HAVE_AUDIT
        " +AUDIT"
#अन्यथा
        " -AUDIT"
#endif

#जर HAVE_SELINUX
        " +SELINUX"
#अन्यथा
        " -SELINUX"
#endif

#जर HAVE_APPARMOR
        " +APPARMOR"
#अन्यथा
        " -APPARMOR"
#endif

#जर ENABLE_IMA
        " +IMA"
#अन्यथा
        " -IMA"
#endif

#जर ENABLE_IPE
        " +IPE"
#अन्यथा
        " -IPE"
#endif

#जर ENABLE_SMACK
        " +SMACK"
#अन्यथा
        " -SMACK"
#endif

#जर HAVE_SECCOMP
        " +SECCOMP"
#अन्यथा
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#जर HAVE_GNUTLS
        " +GNUTLS"
#अन्यथा
        " -GNUTLS"
#endif

#जर HAVE_OPENSSL
        " +OPENSSL"
#अन्यथा
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#जर HAVE_ACL
        " +ACL"
#अन्यथा
        " -ACL"
#endif

#जर HAVE_BLKID
        " +BLKID"
#अन्यथा
        " -BLKID"
#endif

#जर HAVE_LIBCURL
        " +CURL"
#अन्यथा
        " -CURL"
#endif

#जर HAVE_ELFUTILS
        " +ELFUTILS"
#अन्यथा
        " -ELFUTILS"
#endif

#जर HAVE_LIBFIDO2
        " +FIDO2"
#अन्यथा
        " -FIDO2"
#endif

#जर HAVE_LIBIDN2
        " +IDN2"
#अन्यथा
        " -IDN2"
#endif

#जर HAVE_KMOD
        " +KMOD"
#अन्यथा
        " -KMOD"
#endif

#जर HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#अन्यथा
        " -LIBCRYPTSETUP"
#endif

#जर HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#अन्यथा
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#जर HAVE_LIBFDISK
        " +LIBFDISK"
#अन्यथा
        " -LIBFDISK"
#endif

#जर HAVE_PCRE2
        " +PCRE2"
#अन्यथा
        " -PCRE2"
#endif

#जर HAVE_PWQUALITY
        " +PWQUALITY"
#अन्यथा
        " -PWQUALITY"
#endif

#जर HAVE_P11KIT
        " +P11KIT"
#अन्यथा
        " -P11KIT"
#endif

#जर HAVE_QRENCODE
        " +QRENCODE"
#अन्यथा
        " -QRENCODE"
#endif

#जर HAVE_TPM2
        " +TPM2"
#अन्यथा
        " -TPM2"
#endif

        /* compressors */

#जर HAVE_BZIP2
        " +BZIP2"
#अन्यथा
        " -BZIP2"
#endif

#जर HAVE_LZ4
        " +LZ4"
#अन्यथा
        " -LZ4"
#endif

#जर HAVE_XZ
        " +XZ"
#अन्यथा
        " -XZ"
#endif

#जर HAVE_ZLIB
        " +ZLIB"
#अन्यथा
        " -ZLIB"
#endif

#जर HAVE_ZSTD
        " +ZSTD"
#अन्यथा
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#जर BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#अन्यथा
        " -BPF_FRAMEWORK"
#endif

#जर HAVE_VMLINUX_H
        " +BTF"
#अन्यथा
        " -BTF"
#endif

#जर HAVE_XKBCOMMON
        " +XKBCOMMON"
#अन्यथा
        " -XKBCOMMON"
#endif

#जर ENABLE_UTMP
        " +UTMP"
#अन्यथा
        " -UTMP"
#endif

#जर HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#अन्यथा
        " -LIBARCHIVE"
#endif

#जर EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#किंवाजर EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#अन्यथा
        " -EXPERIMENTAL"
#endif
        ;

स्थिर अक्षर* systemd_features_with_color(शून्य) {
        स्थिरांक अक्षर *p = systemd_features;
        _cleanup_free_ अक्षर *ret = रिक्त;
        पूर्णांक r;

        साठी (;;) {
                _cleanup_free_ अक्षर *word = रिक्त;
                अक्षर *q;

                r = extract_first_word(&p, &word, रिक्त, 0);
                जर (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        परत रिक्त;
                }
                जर (r == 0)
                        परत TAKE_PTR(ret);

                जर (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        परत रिक्त;
                }

                जर (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                अन्यथा जर (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                अन्यथा
                        q = strextend(&ret, word);
                जर (!q) {
                        log_oom_warning();
                        परत रिक्त;
                }
        }
}

पूर्णांक version(शून्य) {
        _cleanup_free_ अक्षर *b = रिक्त;

        जर (colors_enabled())
                b = systemd_features_with_color();

        म_लिखो("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        परत 0;
}

पूर्णांक version_only(शून्य) {
        /* Used by udev. Dracut relies on the version being a single integer */
        माला_दो(PROJECT_VERSION_STR);
        परत 0;
}

bool experimental_enabled(शून्य) {
#जर EXPERIMENTAL == EXPERIMENTAL_DISABLED
        परत false;
#अन्यथा
        पूर्णांक r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        जर (r >= 0)
                परत r;
        जर (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        परत EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
