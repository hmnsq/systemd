/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "env-util.h"
#ضمن "errno-util.h"
#ضمن "initrd-util.h"
#ضمن "log.h"

ساكن صحيح saved_اندريون_initrd = -1;

bool اندريون_initrd(فراغ) {
        صحيح r;

        إذا (saved_اندريون_initrd >= 0)
                أرجع saved_اندريون_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        إذا (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        إذا (r >= 0)
                saved_اندريون_initrd = r > 0;
        وإلا {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                إذا (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_اندريون_initrd = r >= 0;
        }

        أرجع saved_اندريون_initrd;
}

فراغ اندريون_initrd_force(bool قدر) {
        saved_اندريون_initrd = قدر;
}

صحيح اندريون_پهريون_boot(فراغ) {
        ساكن صحيح پهريون_boot_env_parse_cached = -1;
        صحيح r;

        إذا (پهريون_boot_env_parse_cached >= 0)
                أرجع پهريون_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        إذا (r >= 0)
                أرجع (پهريون_boot_env_parse_cached = r);
        إذا (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        إذا (r >= 0)
                أرجع true;
        إذا (r == -ENOENT)
                أرجع false;

        أرجع log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
