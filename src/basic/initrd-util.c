/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "initrd-util.h"
#અન્તર્ભાવ "log.h"

સ્થિર પૂર્ણાંક saved_in_initrd = -1;

bool in_initrd(શૂન્ય) {
        પૂર્ણાંક r;

        જો (saved_in_initrd >= 0)
                પાછા saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        જો (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        જો (r >= 0)
                saved_in_initrd = r > 0;
        નહીંતર {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                જો (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        પાછા saved_in_initrd;
}

શૂન્ય in_initrd_force(bool કિંમત) {
        saved_in_initrd = કિંમત;
}

પૂર્ણાંક in_first_boot(શૂન્ય) {
        સ્થિર પૂર્ણાંક first_boot_env_parse_cached = -1;
        પૂર્ણાંક r;

        જો (first_boot_env_parse_cached >= 0)
                પાછા first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        જો (r >= 0)
                પાછા (first_boot_env_parse_cached = r);
        જો (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        જો (r >= 0)
                પાછા true;
        જો (r == -ENOENT)
                પાછા false;

        પાછા log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
