/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "ansi-color.h"
#அந்தர்பாவ "build.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "version.h"

மாறிலி எழுத்து* மாறிலி systemd_features =

        /* PAM and MAC frameworks */

#எனில் HAVE_PAM
        "+PAM"
#இல்லையெனில்
        "-PAM"
#endif

#எனில் HAVE_AUDIT
        " +AUDIT"
#இல்லையெனில்
        " -AUDIT"
#endif

#எனில் HAVE_SELINUX
        " +SELINUX"
#இல்லையெனில்
        " -SELINUX"
#endif

#எனில் HAVE_APPARMOR
        " +APPARMOR"
#இல்லையெனில்
        " -APPARMOR"
#endif

#எனில் ENABLE_IMA
        " +IMA"
#இல்லையெனில்
        " -IMA"
#endif

#எனில் ENABLE_IPE
        " +IPE"
#இல்லையெனில்
        " -IPE"
#endif

#எனில் ENABLE_SMACK
        " +SMACK"
#இல்லையெனில்
        " -SMACK"
#endif

#எனில் HAVE_SECCOMP
        " +SECCOMP"
#இல்லையெனில்
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#எனில் HAVE_GNUTLS
        " +GNUTLS"
#இல்லையெனில்
        " -GNUTLS"
#endif

#எனில் HAVE_OPENSSL
        " +OPENSSL"
#இல்லையெனில்
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#எனில் HAVE_ACL
        " +ACL"
#இல்லையெனில்
        " -ACL"
#endif

#எனில் HAVE_BLKID
        " +BLKID"
#இல்லையெனில்
        " -BLKID"
#endif

#எனில் HAVE_LIBCURL
        " +CURL"
#இல்லையெனில்
        " -CURL"
#endif

#எனில் HAVE_ELFUTILS
        " +ELFUTILS"
#இல்லையெனில்
        " -ELFUTILS"
#endif

#எனில் HAVE_LIBFIDO2
        " +FIDO2"
#இல்லையெனில்
        " -FIDO2"
#endif

#எனில் HAVE_LIBIDN2
        " +IDN2"
#இல்லையெனில்
        " -IDN2"
#endif

#எனில் HAVE_KMOD
        " +KMOD"
#இல்லையெனில்
        " -KMOD"
#endif

#எனில் HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#இல்லையெனில்
        " -LIBCRYPTSETUP"
#endif

#எனில் HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#இல்லையெனில்
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#எனில் HAVE_LIBFDISK
        " +LIBFDISK"
#இல்லையெனில்
        " -LIBFDISK"
#endif

#எனில் HAVE_PCRE2
        " +PCRE2"
#இல்லையெனில்
        " -PCRE2"
#endif

#எனில் HAVE_PWQUALITY
        " +PWQUALITY"
#இல்லையெனில்
        " -PWQUALITY"
#endif

#எனில் HAVE_P11KIT
        " +P11KIT"
#இல்லையெனில்
        " -P11KIT"
#endif

#எனில் HAVE_QRENCODE
        " +QRENCODE"
#இல்லையெனில்
        " -QRENCODE"
#endif

#எனில் HAVE_TPM2
        " +TPM2"
#இல்லையெனில்
        " -TPM2"
#endif

        /* compressors */

#எனில் HAVE_BZIP2
        " +BZIP2"
#இல்லையெனில்
        " -BZIP2"
#endif

#எனில் HAVE_LZ4
        " +LZ4"
#இல்லையெனில்
        " -LZ4"
#endif

#எனில் HAVE_XZ
        " +XZ"
#இல்லையெனில்
        " -XZ"
#endif

#எனில் HAVE_ZLIB
        " +ZLIB"
#இல்லையெனில்
        " -ZLIB"
#endif

#எனில் HAVE_ZSTD
        " +ZSTD"
#இல்லையெனில்
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#எனில் BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#இல்லையெனில்
        " -BPF_FRAMEWORK"
#endif

#எனில் HAVE_VMLINUX_H
        " +BTF"
#இல்லையெனில்
        " -BTF"
#endif

#எனில் HAVE_XKBCOMMON
        " +XKBCOMMON"
#இல்லையெனில்
        " -XKBCOMMON"
#endif

#எனில் ENABLE_UTMP
        " +UTMP"
#இல்லையெனில்
        " -UTMP"
#endif

#எனில் HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#இல்லையெனில்
        " -LIBARCHIVE"
#endif

#எனில் EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#அல்லதெனில் EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#இல்லையெனில்
        " -EXPERIMENTAL"
#endif
        ;

நிலையான எழுத்து* systemd_features_with_color(வெறுமை) {
        மாறிலி எழுத்து *p = systemd_features;
        _cleanup_free_ எழுத்து *ret = வெற்று;
        முழு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *word = வெற்று;
                எழுத்து *q;

                r = extract_first_word(&p, &word, வெற்று, 0);
                எனில் (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        திரும்பு வெற்று;
                }
                எனில் (r == 0)
                        திரும்பு TAKE_PTR(ret);

                எனில் (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        திரும்பு வெற்று;
                }

                எனில் (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                இல்லையெனில் எனில் (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                இல்லையெனில்
                        q = strextend(&ret, word);
                எனில் (!q) {
                        log_oom_warning();
                        திரும்பு வெற்று;
                }
        }
}

முழு version(வெறுமை) {
        _cleanup_free_ எழுத்து *b = வெற்று;

        எனில் (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        திரும்பு 0;
}

முழு version_only(வெறுமை) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        திரும்பு 0;
}

bool experimental_enabled(வெறுமை) {
#எனில் EXPERIMENTAL == EXPERIMENTAL_DISABLED
        திரும்பு false;
#இல்லையெனில்
        முழு r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        எனில் (r >= 0)
                திரும்பு r;
        எனில் (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        திரும்பு EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
