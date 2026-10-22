/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಯದಿ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#endif

#ಅನ್ತರ್ಭಾವ "uid-classification.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಸ್ಥಿರ ಧ್ರುವ UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#ಯದಿ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_alloc_uid(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *t, uid_t *ret_uid) {
        uid_t uid;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_uid);

        r = parse_uid(t, &uid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        ಯದಿ (uid == 0)
                uid = 1;

        *ret_uid = uid;
        ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಪೂರ್ಣಾಙ್ಕ read_login_defs(UGIDAllocationRange *ret_defs, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು) {
        ಪ್ರತಿಜ್ಞಾ(ret_defs);

#ಯದಿ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        UGIDAllocationRange defs;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, ಬೇರು, CHASE_PREFIX_ಬೇರು, "re", ರಿಕ್ತ, &f);
        ಯದಿ (r == -ENOENT)
                ಗಚ್ಛ defaults;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
                ವರ್ಣ *t;

                r = read_ಸಾಲು(f, LINE_MAX, &ಸಾಲು);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to read %s: %m", path);
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ ((t = first_word(ಸಾಲು, "SYS_UID_MIN")))
                        (ಶೂನ್ಯ) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                ಅನ್ಯಥಾ ಯದಿ ((t = first_word(ಸಾಲು, "SYS_UID_MAX")))
                        (ಶೂನ್ಯ) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                ಅನ್ಯಥಾ ಯದಿ ((t = first_word(ಸಾಲು, "SYS_GID_MIN")))
                        (ಶೂನ್ಯ) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                ಅನ್ಯಥಾ ಯದಿ ((t = first_word(ಸಾಲು, "SYS_GID_MAX")))
                        (ಶೂನ್ಯ) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        ಯದಿ (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        ಯದಿ (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        ಪ್ರತ್ಯಾಗಮ 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ UGIDAllocationRange *acquire_ugid_allocation_range(ಶೂನ್ಯ) {
#ಯದಿ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        ಸ್ಥಿರ thread_local UGIDAllocationRange defs;
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಙ್ಕ initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        ಯದಿ (initialized == 0)
                initialized = read_login_defs(&defs, ರಿಕ್ತ, ರಿಕ್ತ) < 0 ? -1 : 1;
        ಯದಿ (initialized < 0)
                ಪ್ರತ್ಯಾಗಮ &default_ugid_allocation_range;

        ಪ್ರತ್ಯಾಗಮ &defs;

#endif
        ಪ್ರತ್ಯಾಗಮ &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        ಧ್ರುವ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        ಪ್ರತ್ಯಾಗಮ uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ಧ್ರುವ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        ಪ್ರತ್ಯಾಗಮ gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        ಪ್ರತ್ಯಾಗಮ uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
