/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "initrd-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ saved_ಒಳ_initrd = -1;

bool ಒಳ_initrd(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (saved_ಒಳ_initrd >= 0)
                ಹಿಂತಿರುಗಿ saved_ಒಳ_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        ಆದರೆ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        ಆದರೆ (r >= 0)
                saved_ಒಳ_initrd = r > 0;
        ಇಲ್ಲದಿದ್ದರೆ {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                ಆದರೆ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_ಒಳ_initrd = r >= 0;
        }

        ಹಿಂತಿರುಗಿ saved_ಒಳ_initrd;
}

ಶೂನ್ಯ ಒಳ_initrd_force(bool ಮೌಲ್ಯ) {
        saved_ಒಳ_initrd = ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಂಕ ಒಳ_ಮೊದಲ_boot(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ_boot_env_parse_cached = -1;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (ಮೊದಲ_boot_env_parse_cached >= 0)
                ಹಿಂತಿರುಗಿ ಮೊದಲ_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ (ಮೊದಲ_boot_env_parse_cached = r);
        ಆದರೆ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ true;
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
