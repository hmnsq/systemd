/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಆದರೆ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "uid-classification.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UGIDAllocationRange default_ugid_ಹಂಚಿಕೆ_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#ಆದರೆ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_alloc_uid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, uid_t *ret_uid) {
        uid_t uid;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_uid);

        r = parse_uid(t, &uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", ಮಾರ್ಗ, name, t);
        ಆದರೆ (uid == 0)
                uid = 1;

        *ret_uid = uid;
        ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ read_login_defs(UGIDAllocationRange *ret_defs, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು) {
        ಪ್ರತಿಜ್ಞಾ(ret_defs);

#ಆದರೆ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        UGIDAllocationRange defs;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!ಮಾರ್ಗ)
                ಮಾರ್ಗ = "/etc/login.defs";

        r = chase_and_fopen_unlocked(ಮಾರ್ಗ, ಬೇರು, CHASE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು, "re", ಖಾಲಿ, &f);
        ಆದರೆ (r == -ENOENT)
                ಹೋಗು defaults;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to open %s: %m", ಮಾರ್ಗ);

        defs = default_ugid_ಹಂಚಿಕೆ_range;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
                ಅಕ್ಷರ *t;

                r = read_ಸಾಲು(f, LINE_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to read %s: %m", ಮಾರ್ಗ);
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ ((t = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, "SYS_UID_MIN")))
                        (ಶೂನ್ಯ) parse_alloc_uid(ಮಾರ್ಗ, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((t = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, "SYS_UID_MAX")))
                        (ಶೂನ್ಯ) parse_alloc_uid(ಮಾರ್ಗ, "SYS_UID_MAX", t, &defs.system_uid_max);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((t = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, "SYS_GID_MIN")))
                        (ಶೂನ್ಯ) parse_alloc_uid(ಮಾರ್ಗ, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((t = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, "SYS_GID_MAX")))
                        (ಶೂನ್ಯ) parse_alloc_uid(ಮಾರ್ಗ, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        ಆದರೆ (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", ಮಾರ್ಗ);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        ಆದರೆ (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", ಮಾರ್ಗ);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        ಹಿಂತಿರುಗಿ 1;
defaults:
#ಆದರೆ_ಅಂತ್ಯ
        *ret_defs = default_ugid_ಹಂಚಿಕೆ_range;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ UGIDAllocationRange *acquire_ugid_ಹಂಚಿಕೆ_range(ಶೂನ್ಯ) {
#ಆದರೆ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        ಸ್ಥಿರ thread_local UGIDAllocationRange defs;
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಂಕ initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        ಆದರೆ (initialized == 0)
                initialized = read_login_defs(&defs, ಖಾಲಿ, ಖಾಲಿ) < 0 ? -1 : 1;
        ಆದರೆ (initialized < 0)
                ಹಿಂತಿರುಗಿ &default_ugid_ಹಂಚಿಕೆ_range;

        ಹಿಂತಿರುಗಿ &defs;

#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ &default_ugid_ಹಂಚಿಕೆ_range;
}

bool uid_is_system(uid_t uid) {
        ಸ್ಥಿರಾಂಕ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_ಹಂಚಿಕೆ_range());

        ಹಿಂತಿರುಗಿ uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ಸ್ಥಿರಾಂಕ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_ಹಂಚಿಕೆ_range());

        ಹಿಂತಿರುಗಿ gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        ಹಿಂತಿರುಗಿ uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
