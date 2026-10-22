/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "ansi-color.h"
#समावेश "build.h"
#समावेश "env-util.h"
#समावेश "extract-word.h"
#समावेश "log.h"
#समावेश "string-util.h"
#समावेश "version.h"

स्थिर अक्षर* स्थिर systemd_features =

        /* PAM and MAC frameworks */

#अगर HAVE_PAM
        "+PAM"
#अन्यथा
        "-PAM"
#पूर्ण_अगर

#अगर HAVE_AUDIT
        " +AUDIT"
#अन्यथा
        " -AUDIT"
#पूर्ण_अगर

#अगर HAVE_SELINUX
        " +SELINUX"
#अन्यथा
        " -SELINUX"
#पूर्ण_अगर

#अगर HAVE_APPARMOR
        " +APPARMOR"
#अन्यथा
        " -APPARMOR"
#पूर्ण_अगर

#अगर ENABLE_IMA
        " +IMA"
#अन्यथा
        " -IMA"
#पूर्ण_अगर

#अगर ENABLE_IPE
        " +IPE"
#अन्यथा
        " -IPE"
#पूर्ण_अगर

#अगर ENABLE_SMACK
        " +SMACK"
#अन्यथा
        " -SMACK"
#पूर्ण_अगर

#अगर HAVE_SECCOMP
        " +SECCOMP"
#अन्यथा
        " -SECCOMP"
#पूर्ण_अगर

        /* cryptographic libraries */

#अगर HAVE_GNUTLS
        " +GNUTLS"
#अन्यथा
        " -GNUTLS"
#पूर्ण_अगर

#अगर HAVE_OPENSSL
        " +OPENSSL"
#अन्यथा
        " -OPENSSL"
#पूर्ण_अगर

        /* all other libraries, sorted alphabetically */

#अगर HAVE_ACL
        " +ACL"
#अन्यथा
        " -ACL"
#पूर्ण_अगर

#अगर HAVE_BLKID
        " +BLKID"
#अन्यथा
        " -BLKID"
#पूर्ण_अगर

#अगर HAVE_LIBCURL
        " +CURL"
#अन्यथा
        " -CURL"
#पूर्ण_अगर

#अगर HAVE_ELFUTILS
        " +ELFUTILS"
#अन्यथा
        " -ELFUTILS"
#पूर्ण_अगर

#अगर HAVE_LIBFIDO2
        " +FIDO2"
#अन्यथा
        " -FIDO2"
#पूर्ण_अगर

#अगर HAVE_LIBIDN2
        " +IDN2"
#अन्यथा
        " -IDN2"
#पूर्ण_अगर

#अगर HAVE_KMOD
        " +KMOD"
#अन्यथा
        " -KMOD"
#पूर्ण_अगर

#अगर HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#अन्यथा
        " -LIBCRYPTSETUP"
#पूर्ण_अगर

#अगर HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#अन्यथा
        " -LIBCRYPTSETUP_PLUGINS"
#पूर्ण_अगर

#अगर HAVE_LIBFDISK
        " +LIBFDISK"
#अन्यथा
        " -LIBFDISK"
#पूर्ण_अगर

#अगर HAVE_PCRE2
        " +PCRE2"
#अन्यथा
        " -PCRE2"
#पूर्ण_अगर

#अगर HAVE_PWQUALITY
        " +PWQUALITY"
#अन्यथा
        " -PWQUALITY"
#पूर्ण_अगर

#अगर HAVE_P11KIT
        " +P11KIT"
#अन्यथा
        " -P11KIT"
#पूर्ण_अगर

#अगर HAVE_QRENCODE
        " +QRENCODE"
#अन्यथा
        " -QRENCODE"
#पूर्ण_अगर

#अगर HAVE_TPM2
        " +TPM2"
#अन्यथा
        " -TPM2"
#पूर्ण_अगर

        /* compressors */

#अगर HAVE_BZIP2
        " +BZIP2"
#अन्यथा
        " -BZIP2"
#पूर्ण_अगर

#अगर HAVE_LZ4
        " +LZ4"
#अन्यथा
        " -LZ4"
#पूर्ण_अगर

#अगर HAVE_XZ
        " +XZ"
#अन्यथा
        " -XZ"
#पूर्ण_अगर

#अगर HAVE_ZLIB
        " +ZLIB"
#अन्यथा
        " -ZLIB"
#पूर्ण_अगर

#अगर HAVE_ZSTD
        " +ZSTD"
#अन्यथा
        " -ZSTD"
#पूर्ण_अगर

        /* other stuff that doesn't fit above */

#अगर BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#अन्यथा
        " -BPF_FRAMEWORK"
#पूर्ण_अगर

#अगर HAVE_VMLINUX_H
        " +BTF"
#अन्यथा
        " -BTF"
#पूर्ण_अगर

#अगर HAVE_XKBCOMMON
        " +XKBCOMMON"
#अन्यथा
        " -XKBCOMMON"
#पूर्ण_अगर

#अगर ENABLE_UTMP
        " +UTMP"
#अन्यथा
        " -UTMP"
#पूर्ण_अगर

#अगर HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#अन्यथा
        " -LIBARCHIVE"
#पूर्ण_अगर

#अगर EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#या_अगर EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#अन्यथा
        " -EXPERIMENTAL"
#पूर्ण_अगर
        ;

अटल अक्षर* systemd_features_with_color(व्योम) {
        स्थिर अक्षर *p = systemd_features;
        _cleanup_free_ अक्षर *ret = शून्य;
        पूर्णांक r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *word = शून्य;
                अक्षर *q;

                r = extract_first_word(&p, &word, शून्य, 0);
                अगर (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        वापस शून्य;
                }
                अगर (r == 0)
                        वापस TAKE_PTR(ret);

                अगर (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        वापस शून्य;
                }

                अगर (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                अन्यथा अगर (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                अन्यथा
                        q = strextend(&ret, word);
                अगर (!q) {
                        log_oom_warning();
                        वापस शून्य;
                }
        }
}

पूर्णांक version(व्योम) {
        _cleanup_free_ अक्षर *b = शून्य;

        अगर (colors_enabled())
                b = systemd_features_with_color();

        म_लिखो("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        वापस 0;
}

पूर्णांक version_only(व्योम) {
        /* Used by udev. Dracut relies on the version being a single integer */
        माला_दो(PROJECT_VERSION_STR);
        वापस 0;
}

bool experimental_enabled(व्योम) {
#अगर EXPERIMENTAL == EXPERIMENTAL_DISABLED
        वापस false;
#अन्यथा
        पूर्णांक r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        अगर (r >= 0)
                वापस r;
        अगर (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        वापस EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#पूर्ण_अगर
}
