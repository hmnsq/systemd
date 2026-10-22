/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "env-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "initrd-util.h"
#अन्तर्भाव "log.h"

स्थिर पूर्णाङ्क saved_in_initrd = -1;

bool in_initrd(शून्य) {
        पूर्णाङ्क r;

        जइ (saved_in_initrd >= 0)
                प्रत्यागम saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        जइ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        जइ (r >= 0)
                saved_in_initrd = r > 0;
        अण्णहा {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                जइ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        प्रत्यागम saved_in_initrd;
}

शून्य in_initrd_force(bool मानम्) {
        saved_in_initrd = मानम्;
}

पूर्णाङ्क in_first_boot(शून्य) {
        स्थिर पूर्णाङ्क first_boot_env_parse_cached = -1;
        पूर्णाङ्क r;

        जइ (first_boot_env_parse_cached >= 0)
                प्रत्यागम first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        जइ (r >= 0)
                प्रत्यागम (first_boot_env_parse_cached = r);
        जइ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        जइ (r >= 0)
                प्रत्यागम true;
        जइ (r == -ENOENT)
                प्रत्यागम false;

        प्रत्यागम log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
