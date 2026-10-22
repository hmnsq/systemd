/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <unistd.h>

#అన్తర్భావ "env-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "initrd-util.h"
#అన్తర్భావ "log.h"

స్థిర పూర్ణాంకం saved_లోపల_initrd = -1;

bool లోపల_initrd(శూన్య) {
        పూర్ణాంకం r;

        ఒకవేళ (saved_లోపల_initrd >= 0)
                తిరిగి saved_లోపల_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        ఒకవేళ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        ఒకవేళ (r >= 0)
                saved_లోపల_initrd = r > 0;
        లేకపోతే {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                ఒకవేళ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_లోపల_initrd = r >= 0;
        }

        తిరిగి saved_లోపల_initrd;
}

శూన్య లోపల_initrd_force(bool విలువ) {
        saved_లోపల_initrd = విలువ;
}

పూర్ణాంకం లోపల_మొదటి_boot(శూన్య) {
        స్థిర పూర్ణాంకం మొదటి_boot_env_parse_cached = -1;
        పూర్ణాంకం r;

        ఒకవేళ (మొదటి_boot_env_parse_cached >= 0)
                తిరిగి మొదటి_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        ఒకవేళ (r >= 0)
                తిరిగి (మొదటి_boot_env_parse_cached = r);
        ఒకవేళ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        ఒకవేళ (r >= 0)
                తిరిగి true;
        ఒకవేళ (r == -ENOENT)
                తిరిగి false;

        తిరిగి log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
