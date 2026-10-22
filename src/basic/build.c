/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "ansi-color.h"
#شامل "build.h"
#شامل "env-util.h"
#شامل "extract-word.h"
#شامل "log.h"
#شامل "string-util.h"
#شامل "version.h"

ثابت نویسه* ثابت systemd_features =

        /* PAM and MAC frameworks */

#اگر HAVE_PAM
        "+PAM"
#وگرنه
        "-PAM"
#endif

#اگر HAVE_AUDIT
        " +AUDIT"
#وگرنه
        " -AUDIT"
#endif

#اگر HAVE_SELINUX
        " +SELINUX"
#وگرنه
        " -SELINUX"
#endif

#اگر HAVE_APPARMOR
        " +APPARMOR"
#وگرنه
        " -APPARMOR"
#endif

#اگر ENABLE_IMA
        " +IMA"
#وگرنه
        " -IMA"
#endif

#اگر ENABLE_IPE
        " +IPE"
#وگرنه
        " -IPE"
#endif

#اگر ENABLE_SMACK
        " +SMACK"
#وگرنه
        " -SMACK"
#endif

#اگر HAVE_SECCOMP
        " +SECCOMP"
#وگرنه
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#اگر HAVE_GNUTLS
        " +GNUTLS"
#وگرنه
        " -GNUTLS"
#endif

#اگر HAVE_OPENSSL
        " +OPENSSL"
#وگرنه
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#اگر HAVE_ACL
        " +ACL"
#وگرنه
        " -ACL"
#endif

#اگر HAVE_BLKID
        " +BLKID"
#وگرنه
        " -BLKID"
#endif

#اگر HAVE_LIBCURL
        " +CURL"
#وگرنه
        " -CURL"
#endif

#اگر HAVE_ELFUTILS
        " +ELFUTILS"
#وگرنه
        " -ELFUTILS"
#endif

#اگر HAVE_LIBFIDO2
        " +FIDO2"
#وگرنه
        " -FIDO2"
#endif

#اگر HAVE_LIBIDN2
        " +IDN2"
#وگرنه
        " -IDN2"
#endif

#اگر HAVE_KMOD
        " +KMOD"
#وگرنه
        " -KMOD"
#endif

#اگر HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#وگرنه
        " -LIBCRYPTSETUP"
#endif

#اگر HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#وگرنه
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#اگر HAVE_LIBFDISK
        " +LIBFDISK"
#وگرنه
        " -LIBFDISK"
#endif

#اگر HAVE_PCRE2
        " +PCRE2"
#وگرنه
        " -PCRE2"
#endif

#اگر HAVE_PWQUALITY
        " +PWQUALITY"
#وگرنه
        " -PWQUALITY"
#endif

#اگر HAVE_P11KIT
        " +P11KIT"
#وگرنه
        " -P11KIT"
#endif

#اگر HAVE_QRENCODE
        " +QRENCODE"
#وگرنه
        " -QRENCODE"
#endif

#اگر HAVE_TPM2
        " +TPM2"
#وگرنه
        " -TPM2"
#endif

        /* compressors */

#اگر HAVE_BZIP2
        " +BZIP2"
#وگرنه
        " -BZIP2"
#endif

#اگر HAVE_LZ4
        " +LZ4"
#وگرنه
        " -LZ4"
#endif

#اگر HAVE_XZ
        " +XZ"
#وگرنه
        " -XZ"
#endif

#اگر HAVE_ZLIB
        " +ZLIB"
#وگرنه
        " -ZLIB"
#endif

#اگر HAVE_ZSTD
        " +ZSTD"
#وگرنه
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#اگر BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#وگرنه
        " -BPF_FRAMEWORK"
#endif

#اگر HAVE_VMLINUX_H
        " +BTF"
#وگرنه
        " -BTF"
#endif

#اگر HAVE_XKBCOMMON
        " +XKBCOMMON"
#وگرنه
        " -XKBCOMMON"
#endif

#اگر ENABLE_UTMP
        " +UTMP"
#وگرنه
        " -UTMP"
#endif

#اگر HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#وگرنه
        " -LIBARCHIVE"
#endif

#اگر EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#وگرنهاگر EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#وگرنه
        " -EXPERIMENTAL"
#endif
        ;

ایستا نویسه* systemd_features_with_color(تهی) {
        ثابت نویسه *p = systemd_features;
        _cleanup_free_ نویسه *ret = پوچ;
        صحیح r;

        برای (;;) {
                _cleanup_free_ نویسه *word = پوچ;
                نویسه *q;

                r = extract_first_word(&p, &word, پوچ, 0);
                اگر (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        بازگشت پوچ;
                }
                اگر (r == 0)
                        بازگشت TAKE_PTR(ret);

                اگر (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        بازگشت پوچ;
                }

                اگر (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                وگرنه اگر (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                وگرنه
                        q = strextend(&ret, word);
                اگر (!q) {
                        log_oom_warning();
                        بازگشت پوچ;
                }
        }
}

صحیح version(تهی) {
        _cleanup_free_ نویسه *b = پوچ;

        اگر (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        بازگشت 0;
}

صحیح version_only(تهی) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        بازگشت 0;
}

bool experimental_enabled(تهی) {
#اگر EXPERIMENTAL == EXPERIMENTAL_DISABLED
        بازگشت false;
#وگرنه
        صحیح r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        اگر (r >= 0)
                بازگشت r;
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        بازگشت EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
