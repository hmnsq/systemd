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

#အကယ်၍ HAVE_PAM
        "+PAM"
#မဟုတ်ပါက
        "-PAM"
#endif

#အကယ်၍ HAVE_AUDIT
        " +AUDIT"
#မဟုတ်ပါက
        " -AUDIT"
#endif

#အကယ်၍ HAVE_SELINUX
        " +SELINUX"
#မဟုတ်ပါက
        " -SELINUX"
#endif

#အကယ်၍ HAVE_APPARMOR
        " +APPARMOR"
#မဟုတ်ပါက
        " -APPARMOR"
#endif

#အကယ်၍ ENABLE_IMA
        " +IMA"
#မဟုတ်ပါက
        " -IMA"
#endif

#အကယ်၍ ENABLE_IPE
        " +IPE"
#မဟုတ်ပါက
        " -IPE"
#endif

#အကယ်၍ ENABLE_SMACK
        " +SMACK"
#မဟုတ်ပါက
        " -SMACK"
#endif

#အကယ်၍ HAVE_SECCOMP
        " +SECCOMP"
#မဟုတ်ပါက
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#အကယ်၍ HAVE_GNUTLS
        " +GNUTLS"
#မဟုတ်ပါက
        " -GNUTLS"
#endif

#အကယ်၍ HAVE_OPENSSL
        " +OPENSSL"
#မဟုတ်ပါက
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#အကယ်၍ HAVE_ACL
        " +ACL"
#မဟုတ်ပါက
        " -ACL"
#endif

#အကယ်၍ HAVE_BLKID
        " +BLKID"
#မဟုတ်ပါက
        " -BLKID"
#endif

#အကယ်၍ HAVE_LIBCURL
        " +CURL"
#မဟုတ်ပါက
        " -CURL"
#endif

#အကယ်၍ HAVE_ELFUTILS
        " +ELFUTILS"
#မဟုတ်ပါက
        " -ELFUTILS"
#endif

#အကယ်၍ HAVE_LIBFIDO2
        " +FIDO2"
#မဟုတ်ပါက
        " -FIDO2"
#endif

#အကယ်၍ HAVE_LIBIDN2
        " +IDN2"
#မဟုတ်ပါက
        " -IDN2"
#endif

#အကယ်၍ HAVE_KMOD
        " +KMOD"
#မဟုတ်ပါက
        " -KMOD"
#endif

#အကယ်၍ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#မဟုတ်ပါက
        " -LIBCRYPTSETUP"
#endif

#အကယ်၍ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#မဟုတ်ပါက
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#အကယ်၍ HAVE_LIBFDISK
        " +LIBFDISK"
#မဟုတ်ပါက
        " -LIBFDISK"
#endif

#အကယ်၍ HAVE_PCRE2
        " +PCRE2"
#မဟုတ်ပါက
        " -PCRE2"
#endif

#အကယ်၍ HAVE_PWQUALITY
        " +PWQUALITY"
#မဟုတ်ပါက
        " -PWQUALITY"
#endif

#အကယ်၍ HAVE_P11KIT
        " +P11KIT"
#မဟုတ်ပါက
        " -P11KIT"
#endif

#အကယ်၍ HAVE_QRENCODE
        " +QRENCODE"
#မဟုတ်ပါက
        " -QRENCODE"
#endif

#အကယ်၍ HAVE_TPM2
        " +TPM2"
#မဟုတ်ပါက
        " -TPM2"
#endif

        /* compressors */

#အကယ်၍ HAVE_BZIP2
        " +BZIP2"
#မဟုတ်ပါက
        " -BZIP2"
#endif

#အကယ်၍ HAVE_LZ4
        " +LZ4"
#မဟုတ်ပါက
        " -LZ4"
#endif

#အကယ်၍ HAVE_XZ
        " +XZ"
#မဟုတ်ပါက
        " -XZ"
#endif

#အကယ်၍ HAVE_ZLIB
        " +ZLIB"
#မဟုတ်ပါက
        " -ZLIB"
#endif

#အကယ်၍ HAVE_ZSTD
        " +ZSTD"
#မဟုတ်ပါက
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#အကယ်၍ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#မဟုတ်ပါက
        " -BPF_FRAMEWORK"
#endif

#အကယ်၍ HAVE_VMLINUX_H
        " +BTF"
#မဟုတ်ပါက
        " -BTF"
#endif

#အကယ်၍ HAVE_XKBCOMMON
        " +XKBCOMMON"
#မဟုတ်ပါက
        " -XKBCOMMON"
#endif

#အကယ်၍ ENABLE_UTMP
        " +UTMP"
#မဟုတ်ပါက
        " -UTMP"
#endif

#အကယ်၍ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#မဟုတ်ပါက
        " -LIBARCHIVE"
#endif

#အကယ်၍ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#မဟုတ်ပါက
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        ကိန်းပြည့် r;

        အတွက် (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                အကယ်၍ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ပြန်ပေး NULL;
                }
                အကယ်၍ (r == 0)
                        ပြန်ပေး TAKE_PTR(ret);

                အကယ်၍ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ပြန်ပေး NULL;
                }

                အကယ်၍ (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                မဟုတ်ပါက အကယ်၍ (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                မဟုတ်ပါက
                        q = strextend(&ret, word);
                အကယ်၍ (!q) {
                        log_oom_warning();
                        ပြန်ပေး NULL;
                }
        }
}

ကိန်းပြည့် version(void) {
        _cleanup_free_ var_na *b = NULL;

        အကယ်၍ (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ပြန်ပေး 0;
}

ကိန်းပြည့် version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ပြန်ပေး 0;
}

bool experimental_enabled(void) {
#အကယ်၍ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ပြန်ပေး false;
#မဟုတ်ပါက
        ကိန်းပြည့် r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        အကယ်၍ (r >= 0)
                ပြန်ပေး r;
        အကယ်၍ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ပြန်ပေး EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
