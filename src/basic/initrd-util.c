/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "initrd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"

ਸਥਿਰ ਪੂਰਨ saved_ਵਿੱਚ_initrd = -1;

bool ਵਿੱਚ_initrd(ਖਾਲੀ) {
        ਪੂਰਨ r;

        ਜੇ (saved_ਵਿੱਚ_initrd >= 0)
                ਵਾਪਸ saved_ਵਿੱਚ_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        ਜੇ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        ਜੇ (r >= 0)
                saved_ਵਿੱਚ_initrd = r > 0;
        ਨਹੀਂਤਾਂ {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                ਜੇ (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_ਵਿੱਚ_initrd = r >= 0;
        }

        ਵਾਪਸ saved_ਵਿੱਚ_initrd;
}

ਖਾਲੀ ਵਿੱਚ_initrd_force(bool ਮੁੱਲ) {
        saved_ਵਿੱਚ_initrd = ਮੁੱਲ;
}

ਪੂਰਨ ਵਿੱਚ_ਪਹਿਲਾ_boot(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ_boot_env_parse_cached = -1;
        ਪੂਰਨ r;

        ਜੇ (ਪਹਿਲਾ_boot_env_parse_cached >= 0)
                ਵਾਪਸ ਪਹਿਲਾ_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        ਜੇ (r >= 0)
                ਵਾਪਸ (ਪਹਿਲਾ_boot_env_parse_cached = r);
        ਜੇ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        ਜੇ (r >= 0)
                ਵਾਪਸ true;
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ false;

        ਵਾਪਸ log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
