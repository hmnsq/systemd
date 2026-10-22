/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "initrd-util.h"
#അന്തര്ഭാവ "log.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ saved_in_initrd = -1;

bool in_initrd(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (saved_in_initrd >= 0)
                തിരികെ saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        എങ്കിൽ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        എങ്കിൽ (r >= 0)
                saved_in_initrd = r > 0;
        അല്ലെങ്കിൽ {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                എങ്കിൽ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        തിരികെ saved_in_initrd;
}

ശൂന്യം in_initrd_force(bool മൂല്യം) {
        saved_in_initrd = മൂല്യം;
}

പൂർണ്ണസംഖ്യ in_first_boot(ശൂന്യം) {
        സ്ഥിതം പൂർണ്ണസംഖ്യ first_boot_env_parse_cached = -1;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (first_boot_env_parse_cached >= 0)
                തിരികെ first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        എങ്കിൽ (r >= 0)
                തിരികെ (first_boot_env_parse_cached = r);
        എങ്കിൽ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        എങ്കിൽ (r >= 0)
                തിരികെ true;
        എങ്കിൽ (r == -ENOENT)
                തിരികെ false;

        തിരികെ log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
