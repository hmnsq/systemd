/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "env-util.h"
#شامل "errno-util.h"
#شامل "initrd-util.h"
#شامل "log.h"

ساکن صحیح saved_اندر_initrd = -1;

bool اندر_initrd(خالی) {
        صحیح r;

        اگر (saved_اندر_initrd >= 0)
                واپس saved_اندر_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        اگر (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        اگر (r >= 0)
                saved_اندر_initrd = r > 0;
        ورنہ {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                اگر (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_اندر_initrd = r >= 0;
        }

        واپس saved_اندر_initrd;
}

خالی اندر_initrd_force(bool قدر) {
        saved_اندر_initrd = قدر;
}

صحیح اندر_پہلا_boot(خالی) {
        ساکن صحیح پہلا_boot_env_parse_cached = -1;
        صحیح r;

        اگر (پہلا_boot_env_parse_cached >= 0)
                واپس پہلا_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        اگر (r >= 0)
                واپس (پہلا_boot_env_parse_cached = r);
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        اگر (r >= 0)
                واپس true;
        اگر (r == -ENOENT)
                واپس false;

        واپس log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
