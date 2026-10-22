/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "ansi-color.h"
#include "build.h"
#include "env-util.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "version.h"

const var_na* const systemd_features =

        /* PAM and MAC frameworks */

#ប្រសិនបើ HAVE_PAM
        "+PAM"
#បើមិនដូច្នេះ
        "-PAM"
#endif

#ប្រសិនបើ HAVE_AUDIT
        " +AUDIT"
#បើមិនដូច្នេះ
        " -AUDIT"
#endif

#ប្រសិនបើ HAVE_SELINUX
        " +SELINUX"
#បើមិនដូច្នេះ
        " -SELINUX"
#endif

#ប្រសិនបើ HAVE_APPARMOR
        " +APPARMOR"
#បើមិនដូច្នេះ
        " -APPARMOR"
#endif

#ប្រសិនបើ ENABLE_IMA
        " +IMA"
#បើមិនដូច្នេះ
        " -IMA"
#endif

#ប្រសិនបើ ENABLE_IPE
        " +IPE"
#បើមិនដូច្នេះ
        " -IPE"
#endif

#ប្រសិនបើ ENABLE_SMACK
        " +SMACK"
#បើមិនដូច្នេះ
        " -SMACK"
#endif

#ប្រសិនបើ HAVE_SECCOMP
        " +SECCOMP"
#បើមិនដូច្នេះ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ប្រសិនបើ HAVE_GNUTLS
        " +GNUTLS"
#បើមិនដូច្នេះ
        " -GNUTLS"
#endif

#ប្រសិនបើ HAVE_OPENSSL
        " +OPENSSL"
#បើមិនដូច្នេះ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ប្រសិនបើ HAVE_ACL
        " +ACL"
#បើមិនដូច្នេះ
        " -ACL"
#endif

#ប្រសិនបើ HAVE_BLKID
        " +BLKID"
#បើមិនដូច្នេះ
        " -BLKID"
#endif

#ប្រសិនបើ HAVE_LIBCURL
        " +CURL"
#បើមិនដូច្នេះ
        " -CURL"
#endif

#ប្រសិនបើ HAVE_ELFUTILS
        " +ELFUTILS"
#បើមិនដូច្នេះ
        " -ELFUTILS"
#endif

#ប្រសិនបើ HAVE_LIBFIDO2
        " +FIDO2"
#បើមិនដូច្នេះ
        " -FIDO2"
#endif

#ប្រសិនបើ HAVE_LIBIDN2
        " +IDN2"
#បើមិនដូច្នេះ
        " -IDN2"
#endif

#ប្រសិនបើ HAVE_KMOD
        " +KMOD"
#បើមិនដូច្នេះ
        " -KMOD"
#endif

#ប្រសិនបើ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#បើមិនដូច្នេះ
        " -LIBCRYPTSETUP"
#endif

#ប្រសិនបើ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#បើមិនដូច្នេះ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ប្រសិនបើ HAVE_LIBFDISK
        " +LIBFDISK"
#បើមិនដូច្នេះ
        " -LIBFDISK"
#endif

#ប្រសិនបើ HAVE_PCRE2
        " +PCRE2"
#បើមិនដូច្នេះ
        " -PCRE2"
#endif

#ប្រសិនបើ HAVE_PWQUALITY
        " +PWQUALITY"
#បើមិនដូច្នេះ
        " -PWQUALITY"
#endif

#ប្រសិនបើ HAVE_P11KIT
        " +P11KIT"
#បើមិនដូច្នេះ
        " -P11KIT"
#endif

#ប្រសិនបើ HAVE_QRENCODE
        " +QRENCODE"
#បើមិនដូច្នេះ
        " -QRENCODE"
#endif

#ប្រសិនបើ HAVE_TPM2
        " +TPM2"
#បើមិនដូច្នេះ
        " -TPM2"
#endif

        /* compressors */

#ប្រសិនបើ HAVE_BZIP2
        " +BZIP2"
#បើមិនដូច្នេះ
        " -BZIP2"
#endif

#ប្រសិនបើ HAVE_LZ4
        " +LZ4"
#បើមិនដូច្នេះ
        " -LZ4"
#endif

#ប្រសិនបើ HAVE_XZ
        " +XZ"
#បើមិនដូច្នេះ
        " -XZ"
#endif

#ប្រសិនបើ HAVE_ZLIB
        " +ZLIB"
#បើមិនដូច្នេះ
        " -ZLIB"
#endif

#ប្រសិនបើ HAVE_ZSTD
        " +ZSTD"
#បើមិនដូច្នេះ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ប្រសិនបើ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#បើមិនដូច្នេះ
        " -BPF_FRAMEWORK"
#endif

#ប្រសិនបើ HAVE_VMLINUX_H
        " +BTF"
#បើមិនដូច្នេះ
        " -BTF"
#endif

#ប្រសិនបើ HAVE_XKBCOMMON
        " +XKBCOMMON"
#បើមិនដូច្នេះ
        " -XKBCOMMON"
#endif

#ប្រសិនបើ ENABLE_UTMP
        " +UTMP"
#បើមិនដូច្នេះ
        " -UTMP"
#endif

#ប្រសិនបើ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#បើមិនដូច្នេះ
        " -LIBARCHIVE"
#endif

#ប្រសិនបើ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#បើមិនដូច្នេះ
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        ចំនួនគត់ r;

        សម្រាប់ (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                ប្រសិនបើ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ត្រឡប់ NULL;
                }
                ប្រសិនបើ (r == 0)
                        ត្រឡប់ TAKE_PTR(ret);

                ប្រសិនបើ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ត្រឡប់ NULL;
                }

                ប្រសិនបើ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                បើមិនដូច្នេះ ប្រសិនបើ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                បើមិនដូច្នេះ
                        q = strextend(&ret, word);
                ប្រសិនបើ (!q) {
                        log_oom_warning();
                        ត្រឡប់ NULL;
                }
        }
}

ចំនួនគត់ version(void) {
        _cleanup_free_ var_na *b = NULL;

        ប្រសិនបើ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ត្រឡប់ 0;
}

ចំនួនគត់ version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ត្រឡប់ 0;
}

bool experimental_enabled(void) {
#ប្រសិនបើ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ត្រឡប់ false;
#បើមិនដូច្នេះ
        ចំនួនគត់ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ប្រសិនបើ (r >= 0)
                ត្រឡប់ r;
        ប្រសិនបើ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ត្រឡប់ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
