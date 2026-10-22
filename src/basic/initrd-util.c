/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "initrd-util.h"
#அந்தர்பாவ "log.h"

நிலையான முழு saved_in_initrd = -1;

bool in_initrd(வெறுமை) {
        முழு r;

        எனில் (saved_in_initrd >= 0)
                திரும்பு saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        எனில் (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        எனில் (r >= 0)
                saved_in_initrd = r > 0;
        இல்லையெனில் {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                எனில் (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        திரும்பு saved_in_initrd;
}

வெறுமை in_initrd_force(bool மதிப்பு) {
        saved_in_initrd = மதிப்பு;
}

முழு in_first_boot(வெறுமை) {
        நிலையான முழு first_boot_env_parse_cached = -1;
        முழு r;

        எனில் (first_boot_env_parse_cached >= 0)
                திரும்பு first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        எனில் (r >= 0)
                திரும்பு (first_boot_env_parse_cached = r);
        எனில் (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        எனில் (r >= 0)
                திரும்பு true;
        எனில் (r == -ENOENT)
                திரும்பு false;

        திரும்பு log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
