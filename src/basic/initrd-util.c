/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "initrd-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_in_initrd = -1;

bool in_initrd(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (saved_in_initrd >= 0)
                ଫେରନ୍ତୁ saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        ଯଦି (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        ଯଦି (r >= 0)
                saved_in_initrd = r > 0;
        ଅନ୍ୟଥା {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                ଯଦି (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        ଫେରନ୍ତୁ saved_in_initrd;
}

ଶୂନ୍ୟ in_initrd_force(bool ମୂଲ୍ୟ) {
        saved_in_initrd = ମୂଲ୍ୟ;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_first_boot(ଶୂନ୍ୟ) {
        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_boot_env_parse_cached = -1;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (first_boot_env_parse_cached >= 0)
                ଫେରନ୍ତୁ first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ (first_boot_env_parse_cached = r);
        ଯଦି (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ true;
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
