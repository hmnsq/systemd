/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "env-util.h"
#ضمن "errno-util.h"
#ضمن "initrd-util.h"
#ضمن "log.h"

ساكن صحيح saved_in_initrd = -1;

bool in_initrd(فراغ) {
        صحيح r;

        אם (saved_in_initrd >= 0)
                החזר saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        אם (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        אם (r >= 0)
                saved_in_initrd = r > 0;
        אחרת {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                אם (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        החזר saved_in_initrd;
}

فراغ in_initrd_force(bool value) {
        saved_in_initrd = value;
}

صحيح in_first_boot(فراغ) {
        ساكن صحيح first_boot_env_parse_cached = -1;
        صحيح r;

        אם (first_boot_env_parse_cached >= 0)
                החזר first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        אם (r >= 0)
                החזר (first_boot_env_parse_cached = r);
        אם (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        אם (r >= 0)
                החזר true;
        אם (r == -ENOENT)
                החזר false;

        החזר log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
