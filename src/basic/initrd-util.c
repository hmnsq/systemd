/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "env-util.h"
#شامل "errno-util.h"
#شامل "initrd-util.h"
#شامل "log.h"

ایستا صحیح saved_in_initrd = -1;

bool in_initrd(تهی) {
        صحیح r;

        اگر (saved_in_initrd >= 0)
                بازگشت saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        اگر (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        اگر (r >= 0)
                saved_in_initrd = r > 0;
        وگرنه {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                اگر (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        بازگشت saved_in_initrd;
}

تهی in_initrd_force(bool مقدار) {
        saved_in_initrd = مقدار;
}

صحیح in_first_boot(تهی) {
        ایستا صحیح first_boot_env_parse_cached = -1;
        صحیح r;

        اگر (first_boot_env_parse_cached >= 0)
                بازگشت first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        اگر (r >= 0)
                بازگشت (first_boot_env_parse_cached = r);
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        اگر (r >= 0)
                بازگشت true;
        اگر (r == -ENOENT)
                بازگشت false;

        بازگشت log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
