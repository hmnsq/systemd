/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "env-util.h"
#include "errno-util.h"
#include "initrd-util.h"
#include "log.h"

static 정수 saved_in_initrd = -1;

bool in_initrd(void) {
        정수 r;

        만약 (saved_in_initrd >= 0)
                반환 saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        만약 (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        만약 (r >= 0)
                saved_in_initrd = r > 0;
        아니면 {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                만약 (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        반환 saved_in_initrd;
}

void in_initrd_force(bool value) {
        saved_in_initrd = value;
}

정수 in_first_boot(void) {
        static 정수 first_boot_env_parse_cached = -1;
        정수 r;

        만약 (first_boot_env_parse_cached >= 0)
                반환 first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        만약 (r >= 0)
                반환 (first_boot_env_parse_cached = r);
        만약 (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        만약 (r >= 0)
                반환 true;
        만약 (r == -ENOENT)
                반환 false;

        반환 log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
