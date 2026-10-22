/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "ansi-color.h"
#అన్తర్భావ "build.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "version.h"

స్థిరరాశి అక్షరం* స్థిరరాశి systemd_features =

        /* PAM and MAC frameworks */

#ఒకవేళ HAVE_PAM
        "+PAM"
#లేకపోతే
        "-PAM"
#అయితే_ముగింపు

#ఒకవేళ HAVE_AUDIT
        " +AUDIT"
#లేకపోతే
        " -AUDIT"
#అయితే_ముగింపు

#ఒకవేళ HAVE_SELINUX
        " +SELINUX"
#లేకపోతే
        " -SELINUX"
#అయితే_ముగింపు

#ఒకవేళ HAVE_APPARMOR
        " +APPARMOR"
#లేకపోతే
        " -APPARMOR"
#అయితే_ముగింపు

#ఒకవేళ ENABLE_IMA
        " +IMA"
#లేకపోతే
        " -IMA"
#అయితే_ముగింపు

#ఒకవేళ ENABLE_IPE
        " +IPE"
#లేకపోతే
        " -IPE"
#అయితే_ముగింపు

#ఒకవేళ ENABLE_SMACK
        " +SMACK"
#లేకపోతే
        " -SMACK"
#అయితే_ముగింపు

#ఒకవేళ HAVE_SECCOMP
        " +SECCOMP"
#లేకపోతే
        " -SECCOMP"
#అయితే_ముగింపు

        /* cryptographic libraries */

#ఒకవేళ HAVE_GNUTLS
        " +GNUTLS"
#లేకపోతే
        " -GNUTLS"
#అయితే_ముగింపు

#ఒకవేళ HAVE_OPENSSL
        " +OPENSSL"
#లేకపోతే
        " -OPENSSL"
#అయితే_ముగింపు

        /* all other libraries, sorted alphabetically */

#ఒకవేళ HAVE_ACL
        " +ACL"
#లేకపోతే
        " -ACL"
#అయితే_ముగింపు

#ఒకవేళ HAVE_BLKID
        " +BLKID"
#లేకపోతే
        " -BLKID"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBCURL
        " +CURL"
#లేకపోతే
        " -CURL"
#అయితే_ముగింపు

#ఒకవేళ HAVE_ELFUTILS
        " +ELFUTILS"
#లేకపోతే
        " -ELFUTILS"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBFIDO2
        " +FIDO2"
#లేకపోతే
        " -FIDO2"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBIDN2
        " +IDN2"
#లేకపోతే
        " -IDN2"
#అయితే_ముగింపు

#ఒకవేళ HAVE_KMOD
        " +KMOD"
#లేకపోతే
        " -KMOD"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBCRYPTSETUP
        " +LIBCRYPTSETUP"
#లేకపోతే
        " -LIBCRYPTSETUP"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBCRYPTSETUP_PLUGINS
        " +LIBCRYPTSETUP_PLUGINS"
#లేకపోతే
        " -LIBCRYPTSETUP_PLUGINS"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBFDISK
        " +LIBFDISK"
#లేకపోతే
        " -LIBFDISK"
#అయితే_ముగింపు

#ఒకవేళ HAVE_PCRE2
        " +PCRE2"
#లేకపోతే
        " -PCRE2"
#అయితే_ముగింపు

#ఒకవేళ HAVE_PWQUALITY
        " +PWQUALITY"
#లేకపోతే
        " -PWQUALITY"
#అయితే_ముగింపు

#ఒకవేళ HAVE_P11KIT
        " +P11KIT"
#లేకపోతే
        " -P11KIT"
#అయితే_ముగింపు

#ఒకవేళ HAVE_QRENCODE
        " +QRENCODE"
#లేకపోతే
        " -QRENCODE"
#అయితే_ముగింపు

#ఒకవేళ HAVE_TPM2
        " +TPM2"
#లేకపోతే
        " -TPM2"
#అయితే_ముగింపు

        /* compressors */

#ఒకవేళ HAVE_BZIP2
        " +BZIP2"
#లేకపోతే
        " -BZIP2"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        " +LZ4"
#లేకపోతే
        " -LZ4"
#అయితే_ముగింపు

#ఒకవేళ HAVE_XZ
        " +XZ"
#లేకపోతే
        " -XZ"
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        " +ZLIB"
#లేకపోతే
        " -ZLIB"
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        " +ZSTD"
#లేకపోతే
        " -ZSTD"
#అయితే_ముగింపు

        /* other stuff that doesn't fit above */

#ఒకవేళ BPF_FRAMEWORK
        " +BPF_FRAMEWORK"
#లేకపోతే
        " -BPF_FRAMEWORK"
#అయితే_ముగింపు

#ఒకవేళ HAVE_VMLINUX_H
        " +BTF"
#లేకపోతే
        " -BTF"
#అయితే_ముగింపు

#ఒకవేళ HAVE_XKBCOMMON
        " +XKBCOMMON"
#లేకపోతే
        " -XKBCOMMON"
#అయితే_ముగింపు

#ఒకవేళ ENABLE_UTMP
        " +UTMP"
#లేకపోతే
        " -UTMP"
#అయితే_ముగింపు

#ఒకవేళ HAVE_LIBARCHIVE
        " +LIBARCHIVE"
#లేకపోతే
        " -LIBARCHIVE"
#అయితే_ముగింపు

#ఒకవేళ EXPERIMENTAL == EXPERIMENTAL_ENABLED
        " +EXPERIMENTAL"
#లేదావేళ EXPERIMENTAL == EXPERIMENTAL_ENVVAR
        " +EXPERIMENTAL-ENVVAR"
#లేకపోతే
        " -EXPERIMENTAL"
#అయితే_ముగింపు
        ;

స్థిర అక్షరం* systemd_features_with_color(శూన్య) {
        స్థిరరాశి అక్షరం *p = systemd_features;
        _cleanup_free_ అక్షరం *ret = ఖాళీ;
        పూర్ణాంకం r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *మాట = ఖాళీ;
                అక్షరం *q;

                r = వెలికితీయి_మొదటి_మాట(&p, &మాట, ఖాళీ, 0);
                ఒకవేళ (r < 0) {
                        log_warning_errno(r, "Cannot split features string, ignoring: %m");
                        తిరిగి ఖాళీ;
                }
                ఒకవేళ (r == 0)
                        తిరిగి TAKE_PTR(ret);

                ఒకవేళ (ret && !strextend(&ret, " ")) {
                        log_oom_warning();
                        తిరిగి ఖాళీ;
                }

                ఒకవేళ (మాట[0] == '+')
                        q = strextend(&ret, ANSI_HIGHLIGHT_GREEN, CHAR_గమ్యం_STR(మాట[0]), ANSI_GREEN, మాట+1, ANSI_సాధారణ);
                లేకపోతే ఒకవేళ (మాట[0] == '-')
                        q = strextend(&ret, ANSI_HIGHLIGHT_RED, CHAR_గమ్యం_STR(మాట[0]), ANSI_RED, మాట+1, ANSI_సాధారణ);
                లేకపోతే
                        q = strextend(&ret, మాట);
                ఒకవేళ (!q) {
                        log_oom_warning();
                        తిరిగి ఖాళీ;
                }
        }
}

పూర్ణాంకం సంస్కరణ(శూన్య) {
        _cleanup_free_ అక్షరం *b = ఖాళీ;

        ఒకవేళ (colors_enabled())
                b = systemd_features_with_color();

        రాయి("%ssystemd %i%s (" GIT_సంస్కరణ ")\n%s\n",
               ansi_highlight(), PROJECT_సంస్కరణ, ansi_సాధారణ(),
               b ?: systemd_features);
        తిరిగి 0;
}

పూర్ణాంకం సంస్కరణ_only(శూన్య) {
        /* Used by udev. Dracut relies on the version being a single integer */
        puts(PROJECT_సంస్కరణ_STR);
        తిరిగి 0;
}

bool experimental_enabled(శూన్య) {
#ఒకవేళ EXPERIMENTAL == EXPERIMENTAL_DISABLED
        తిరిగి false;
#లేకపోతే
        పూర్ణాంకం r;

        r = secure_getenv_bool("SYSTEMD_EXPERIMENTAL");
        ఒకవేళ (r >= 0)
                తిరిగి r;
        ఒకవేళ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_EXPERIMENTAL, ignoring: %m");

        తిరిగి EXPERIMENTAL == EXPERIMENTAL_ENABLED;
#అయితే_ముగింపు
}
