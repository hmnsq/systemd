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

#ຖ້າ HAVE_PAM
        "+PAM"
#ບໍ່ດັ່ງນັ້ນ
        "-PAM"
#endif

#ຖ້າ HAVE_AUDIT
        " +AUDIT"
#ບໍ່ດັ່ງນັ້ນ
        " -AUDIT"
#endif

#ຖ້າ HAVE_SELINUX
        " +SELINUX"
#ບໍ່ດັ່ງນັ້ນ
        " -SELINUX"
#endif

#ຖ້າ HAVE_APPARMOR
        " +APPARMOR"
#ບໍ່ດັ່ງນັ້ນ
        " -APPARMOR"
#endif

#ຖ້າ ENABLE_IMA
        " +IMA"
#ບໍ່ດັ່ງນັ້ນ
        " -IMA"
#endif

#ຖ້າ ENABLE_IPE
        " +IPE"
#ບໍ່ດັ່ງນັ້ນ
        " -IPE"
#endif

#ຖ້າ ENABLE_SMACK
        " +SMACK"
#ບໍ່ດັ່ງນັ້ນ
        " -SMACK"
#endif

#ຖ້າ HAVE_SECCOMP
        " +SECCOMP"
#ບໍ່ດັ່ງນັ້ນ
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#ຖ້າ HAVE_GNUTLS
        " +GNUTLS"
#ບໍ່ດັ່ງນັ້ນ
        " -GNUTLS"
#endif

#ຖ້າ HAVE_OPENSSL
        " +OPENSSL"
#ບໍ່ດັ່ງນັ້ນ
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#ຖ້າ HAVE_ACL
        " +ACL"
#ບໍ່ດັ່ງນັ້ນ
        " -ACL"
#endif

#ຖ້າ HAVE_BLKID
        " +BLKID"
#ບໍ່ດັ່ງນັ້ນ
        " -BLKID"
#endif

#ຖ້າ HAVE_LIBCURL
        " +CURL"
#ບໍ່ດັ່ງນັ້ນ
        " -CURL"
#endif

#ຖ້າ HAVE_ELFUTILS
        " +ELFUTILS"
#ບໍ່ດັ່ງນັ້ນ
        " -ELFUTILS"
#endif

#ຖ້າ HAVE_LIBFIDO2
        " +FIDO2"
#ບໍ່ດັ່ງນັ້ນ
        " -FIDO2"
#endif

#ຖ້າ HAVE_LIBIDN2
        " +IDN2"
#ບໍ່ດັ່ງນັ້ນ
        " -IDN2"
#endif

#ຖ້າ HAVE_KMOD
        " +KMOD"
#ບໍ່ດັ່ງນັ້ນ
        " -KMOD"
#endif

#ຖ້າ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#ບໍ່ດັ່ງນັ້ນ
        " -LIBCRYPTSETUP"
#endif

#ຖ້າ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#ບໍ່ດັ່ງນັ້ນ
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#ຖ້າ HAVE_LIBFDISK
        " +LIBFDISK"
#ບໍ່ດັ່ງນັ້ນ
        " -LIBFDISK"
#endif

#ຖ້າ HAVE_PCRE2
        " +PCRE2"
#ບໍ່ດັ່ງນັ້ນ
        " -PCRE2"
#endif

#ຖ້າ HAVE_PWQUALITY
        " +PWQUALITY"
#ບໍ່ດັ່ງນັ້ນ
        " -PWQUALITY"
#endif

#ຖ້າ HAVE_P11KIT
        " +P11KIT"
#ບໍ່ດັ່ງນັ້ນ
        " -P11KIT"
#endif

#ຖ້າ HAVE_QRENCODE
        " +QRENCODE"
#ບໍ່ດັ່ງນັ້ນ
        " -QRENCODE"
#endif

#ຖ້າ HAVE_TPM2
        " +TPM2"
#ບໍ່ດັ່ງນັ້ນ
        " -TPM2"
#endif

        /* compressors */

#ຖ້າ HAVE_BZIP2
        " +BZIP2"
#ບໍ່ດັ່ງນັ້ນ
        " -BZIP2"
#endif

#ຖ້າ HAVE_LZ4
        " +LZ4"
#ບໍ່ດັ່ງນັ້ນ
        " -LZ4"
#endif

#ຖ້າ HAVE_XZ
        " +XZ"
#ບໍ່ດັ່ງນັ້ນ
        " -XZ"
#endif

#ຖ້າ HAVE_ZLIB
        " +ZLIB"
#ບໍ່ດັ່ງນັ້ນ
        " -ZLIB"
#endif

#ຖ້າ HAVE_ZSTD
        " +ZSTD"
#ບໍ່ດັ່ງນັ້ນ
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#ຖ້າ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#ບໍ່ດັ່ງນັ້ນ
        " -BPF_FRAMEWORK"
#endif

#ຖ້າ HAVE_VMLINUX_H
        " +BTF"
#ບໍ່ດັ່ງນັ້ນ
        " -BTF"
#endif

#ຖ້າ HAVE_XKBCOMMON
        " +XKBCOMMON"
#ບໍ່ດັ່ງນັ້ນ
        " -XKBCOMMON"
#endif

#ຖ້າ ENABLE_UTMP
        " +UTMP"
#ບໍ່ດັ່ງນັ້ນ
        " -UTMP"
#endif

#ຖ້າ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#ບໍ່ດັ່ງນັ້ນ
        " -LIBARCHIVE"
#endif

#ຖ້າ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#ບໍ່ດັ່ງນັ້ນ
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        ຈຳນວນເຕັມ r;

        ສຳລັບ (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                ຖ້າ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ສົ່ງຄືນ NULL;
                }
                ຖ້າ (r == 0)
                        ສົ່ງຄືນ TAKE_PTR(ret);

                ຖ້າ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ສົ່ງຄືນ NULL;
                }

                ຖ້າ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                ບໍ່ດັ່ງນັ້ນ ຖ້າ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                ບໍ່ດັ່ງນັ້ນ
                        q = strextend(&ret, word);
                ຖ້າ (!q) {
                        log_oom_warning();
                        ສົ່ງຄືນ NULL;
                }
        }
}

ຈຳນວນເຕັມ version(void) {
        _cleanup_free_ var_na *b = NULL;

        ຖ້າ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ສົ່ງຄືນ 0;
}

bool experimental_enabled(void) {
#ຖ້າ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ສົ່ງຄືນ false;
#ບໍ່ດັ່ງນັ້ນ
        ຈຳນວນເຕັມ r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ຖ້າ (r >= 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ສົ່ງຄືນ EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
