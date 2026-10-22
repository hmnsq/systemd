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

#නම් HAVE_PAM
        "+PAM"
#නැතහොත්
        "-PAM"
#endif

#නම් HAVE_AUDIT
        " +AUDIT"
#නැතහොත්
        " -AUDIT"
#endif

#නම් HAVE_SELINUX
        " +SELINUX"
#නැතහොත්
        " -SELINUX"
#endif

#නම් HAVE_APPARMOR
        " +APPARMOR"
#නැතහොත්
        " -APPARMOR"
#endif

#නම් ENABLE_IMA
        " +IMA"
#නැතහොත්
        " -IMA"
#endif

#නම් ENABLE_IPE
        " +IPE"
#නැතහොත්
        " -IPE"
#endif

#නම් ENABLE_SMACK
        " +SMACK"
#නැතහොත්
        " -SMACK"
#endif

#නම් HAVE_SECCOMP
        " +SECCOMP"
#නැතහොත්
        " -SECCOMP"
#endif

        /* cryptographic libraries */

#නම් HAVE_GNUTLS
        " +GNUTLS"
#නැතහොත්
        " -GNUTLS"
#endif

#නම් HAVE_OPENSSL
        " +OPENSSL"
#නැතහොත්
        " -OPENSSL"
#endif

        /* all other libraries, sorted alphabetically */

#නම් HAVE_ACL
        " +ACL"
#නැතහොත්
        " -ACL"
#endif

#නම් HAVE_BLKID
        " +BLKID"
#නැතහොත්
        " -BLKID"
#endif

#නම් HAVE_LIBCURL
        " +CURL"
#නැතහොත්
        " -CURL"
#endif

#නම් HAVE_ELFUTILS
        " +ELFUTILS"
#නැතහොත්
        " -ELFUTILS"
#endif

#නම් HAVE_LIBFIDO2
        " +FIDO2"
#නැතහොත්
        " -FIDO2"
#endif

#නම් HAVE_LIBIDN2
        " +IDN2"
#නැතහොත්
        " -IDN2"
#endif

#නම් HAVE_KMOD
        " +KMOD"
#නැතහොත්
        " -KMOD"
#endif

#නම් HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#නැතහොත්
        " -LIBCRYPTSETUP"
#endif

#නම් HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#නැතහොත්
        " -LIBCRYPTSETUP_PLUGINS"
#endif

#නම් HAVE_LIBFDISK
        " +LIBFDISK"
#නැතහොත්
        " -LIBFDISK"
#endif

#නම් HAVE_PCRE2
        " +PCRE2"
#නැතහොත්
        " -PCRE2"
#endif

#නම් HAVE_PWQUALITY
        " +PWQUALITY"
#නැතහොත්
        " -PWQUALITY"
#endif

#නම් HAVE_P11KIT
        " +P11KIT"
#නැතහොත්
        " -P11KIT"
#endif

#නම් HAVE_QRENCODE
        " +QRENCODE"
#නැතහොත්
        " -QRENCODE"
#endif

#නම් HAVE_TPM2
        " +TPM2"
#නැතහොත්
        " -TPM2"
#endif

        /* compressors */

#නම් HAVE_BZIP2
        " +BZIP2"
#නැතහොත්
        " -BZIP2"
#endif

#නම් HAVE_LZ4
        " +LZ4"
#නැතහොත්
        " -LZ4"
#endif

#නම් HAVE_XZ
        " +XZ"
#නැතහොත්
        " -XZ"
#endif

#නම් HAVE_ZLIB
        " +ZLIB"
#නැතහොත්
        " -ZLIB"
#endif

#නම් HAVE_ZSTD
        " +ZSTD"
#නැතහොත්
        " -ZSTD"
#endif

        /* other stuff that doesn't fit above */

#නම් BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#නැතහොත්
        " -BPF_FRAMEWORK"
#endif

#නම් HAVE_VMLINUX_H
        " +BTF"
#නැතහොත්
        " -BTF"
#endif

#නම් HAVE_XKBCOMMON
        " +XKBCOMMON"
#නැතහොත්
        " -XKBCOMMON"
#endif

#නම් ENABLE_UTMP
        " +UTMP"
#නැතහොත්
        " -UTMP"
#endif

#නම් HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#නැතහොත්
        " -LIBARCHIVE"
#endif

#නම් EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#elif EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#නැතහොත්
        " -EXPERIMENTAL"
#endif
        ;

static var_na* systemd_features_with_color(void) {
        const var_na *p = systemd_features;
        _cleanup_free_ var_na *ret = NULL;
        පූර්ණ_සංඛ්‍යා r;

        සඳහා (;;) {
                _cleanup_free_ var_na *word = NULL;
                var_na *q;

                r = extract_first_word(&p, &word, NULL, 0);
                නම් (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        ආපසු NULL;
                }
                නම් (r == 0)
                        ආපසු TAKE_PTR(ret);

                නම් (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        ආපසු NULL;
                }

                නම් (word[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_TO_STR(word[0]), ANSI_GREEN, word+1, ANSI_NORMAL);
                නැතහොත් නම් (word[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_TO_STR(word[0]), ANSI_RED, word+1, ANSI_NORMAL);
                නැතහොත්
                        q = strextend(&ret, word);
                නම් (!q) {
                        log_oom_warning();
                        ආපසු NULL;
                }
        }
}

පූර්ණ_සංඛ්‍යා version(void) {
        _cleanup_free_ var_na *b = NULL;

        නම් (colors_enabled())
                b = systemd_features_with_color();

        printf("%ssystemd %i%s (" GIT_VERSION ")\n%s\n",
               ansi_highlight(), PROJECT_VERSION, ansi_normal(),
               b ?: systemd_features);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා version_only(void) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_VERSION_STR);
        ආපසු 0;
}

bool experimental_enabled(void) {
#නම් EXPERIMENTAL == EXPERIMENTAL_DISABLED
        ආපසු false;
#නැතහොත්
        පූර්ණ_සංඛ්‍යා r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        නම් (r >= 0)
                ආපසු r;
        නම් (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        ආපසු EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#endif
}
