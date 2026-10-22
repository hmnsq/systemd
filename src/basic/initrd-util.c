/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "env-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "initrd-util.h"
#अन्तर्भाव "log.h"

स्थिर पूर्णाङ्क saved_अन्तर्_initrd = -1;

bool अन्तर्_initrd(शून्य) {
        पूर्णाङ्क r;

        यदि (saved_अन्तर्_initrd >= 0)
                प्रत्यागम saved_अन्तर्_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        यदि (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        यदि (r >= 0)
                saved_अन्तर्_initrd = r > 0;
        अन्यथा {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                यदि (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_अन्तर्_initrd = r >= 0;
        }

        प्रत्यागम saved_अन्तर्_initrd;
}

शून्य अन्तर्_initrd_force(bool मानम्) {
        saved_अन्तर्_initrd = मानम्;
}

पूर्णाङ्क अन्तर्_first_boot(शून्य) {
        स्थिर पूर्णाङ्क first_boot_env_parse_cached = -1;
        पूर्णाङ्क r;

        यदि (first_boot_env_parse_cached >= 0)
                प्रत्यागम first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        यदि (r >= 0)
                प्रत्यागम (first_boot_env_parse_cached = r);
        यदि (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        यदि (r >= 0)
                प्रत्यागम true;
        यदि (r == -ENOENT)
                प्रत्यागम false;

        प्रत्यागम log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
