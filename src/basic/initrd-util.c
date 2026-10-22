/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "initrd-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ saved_in_initrd = -1;

bool in_initrd(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (saved_in_initrd >= 0)
                ಪ್ರತ್ಯಾಗಮ saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        ಯದಿ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        ಯದಿ (r >= 0)
                saved_in_initrd = r > 0;
        ಅನ್ಯಥಾ {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                ಯದಿ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        ಪ್ರತ್ಯಾಗಮ saved_in_initrd;
}

ಶೂನ್ಯ in_initrd_force(bool ಮೌಲ್ಯ) {
        saved_in_initrd = ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ in_first_boot(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first_boot_env_parse_cached = -1;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (first_boot_env_parse_cached >= 0)
                ಪ್ರತ್ಯಾಗಮ first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ (first_boot_env_parse_cached = r);
        ಯದಿ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ true;
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
