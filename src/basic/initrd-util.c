/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <unistd.h>

#समावेश "env-util.h"
#समावेश "errno-util.h"
#समावेश "initrd-util.h"
#समावेश "log.h"

अटल पूर्णांक saved_in_initrd = -1;

bool in_initrd(व्योम) {
        पूर्णांक r;

        अगर (saved_in_initrd >= 0)
                वापस saved_in_initrd;

        /* If /etc/initrd-release exists, we're in an initrd.
         * This can be overridden by setting SYSTEMD_IN_INITRD=0|1.
         */

        r = secure_getenv_bool("SYSTEMD_IN_INITRD");
        अगर (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_INITRD, ignoring: %m");

        अगर (r >= 0)
                saved_in_initrd = r > 0;
        अन्यथा {
                r = RET_NERRNO(access("/etc/initrd-release", F_OK));
                अगर (r < 0 && r != -ENOENT)
                        log_debug_errno(r, "Failed to check if /etc/initrd-release exists, assuming it does not: %m");
                saved_in_initrd = r >= 0;
        }

        वापस saved_in_initrd;
}

व्योम in_initrd_force(bool मूल्य) {
        saved_in_initrd = मूल्य;
}

पूर्णांक in_first_boot(व्योम) {
        अटल पूर्णांक first_boot_env_parse_cached = -1;
        पूर्णांक r;

        अगर (first_boot_env_parse_cached >= 0)
                वापस first_boot_env_parse_cached;

        r = secure_getenv_bool("SYSTEMD_FIRST_BOOT");
        अगर (r >= 0)
                वापस (first_boot_env_parse_cached = r);
        अगर (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_FIRST_BOOT, ignoring: %m");

        /* This is not cached and must *never* be cached, other parts of systemd write it to signal
         * an update to the first-boot state. */
        r = RET_NERRNO(access("/run/systemd/first-boot", F_OK));
        अगर (r >= 0)
                वापस true;
        अगर (r == -ENOENT)
                वापस false;

        वापस log_debug_errno(r, "Failed to check /run/systemd/first-boot: %m");
}
