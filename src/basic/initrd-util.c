/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "env-util.h"
#include "errno-util.h"
#include "initrd-util.h"
#include "log.h"

static puur_naa_nka saved_in_initrd = -1;

bool in_initrd(void) {
        puur_naa_nka r;

        ᯀᯮᯝ (saved_in_initrd >= 0)
                ᯀᯮᯞ saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        ᯀᯮᯝ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        ᯀᯮᯝ (r >= 0)
                saved_in_initrd = r > 0;
        ᯀᯮᯝᯂᯪ {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                ᯀᯮᯝ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        ᯀᯮᯞ saved_in_initrd;
}

void in_initrd_force(bool value) {
        saved_in_initrd = value;
}

puur_naa_nka in_first_boot(void) {
        static puur_naa_nka first_boot_env_parse_cached = -1;
        puur_naa_nka r;

        ᯀᯮᯝ (first_boot_env_parse_cached >= 0)
                ᯀᯮᯞ first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        ᯀᯮᯝ (r >= 0)
                ᯀᯮᯞ (first_boot_env_parse_cached = r);
        ᯀᯮᯝ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        ᯀᯮᯝ (r >= 0)
                ᯀᯮᯞ true;
        ᯀᯮᯝ (r == -ENOENT)
                ᯀᯮᯞ false;

        ᯀᯮᯞ log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
