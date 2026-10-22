/* SPDX-License-Identifier: LGPL-2.1-or-later */

#जर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#अन्तर्भाव <threads.h>

#अन्तर्भाव "chase.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#endif

#अन्तर्भाव "uid-classification.h"
#अन्तर्भाव "user-util.h"

स्थिर स्थिरांक UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#जर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
स्थिर पूर्णांक parse_alloc_uid(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, स्थिरांक अक्षर *t, uid_t *ret_uid) {
        uid_t uid;
        पूर्णांक r;

        प्रतिज्ञा(ret_uid);

        r = parse_uid(t, &uid);
        जर (r < 0)
                परत log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        जर (uid == 0)
                uid = 1;

        *ret_uid = uid;
        परत 0;
}
#endif

पूर्णांक read_login_defs(UGIDAllocationRange *ret_defs, स्थिरांक अक्षर *path, स्थिरांक अक्षर *मूळ) {
        प्रतिज्ञा(ret_defs);

#जर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ खाता *f = रिक्त;
        UGIDAllocationRange defs;
        पूर्णांक r;

        जर (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, मूळ, CHASE_PREFIX_मूळ, "re", रिक्त, &f);
        जर (r == -ENOENT)
                जा defaults;
        जर (r < 0)
                परत log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        साठी (;;) {
                _cleanup_free_ अक्षर *ओळ = रिक्त;
                अक्षर *t;

                r = read_ओळ(f, LINE_MAX, &ओळ);
                जर (r < 0)
                        परत log_debug_errno(r, "Failed to read %s: %m", path);
                जर (r == 0)
                        थांबा;

                जर ((t = first_word(ओळ, "SYS_UID_MIN")))
                        (शून्य) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                अन्यथा जर ((t = first_word(ओळ, "SYS_UID_MAX")))
                        (शून्य) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                अन्यथा जर ((t = first_word(ओळ, "SYS_GID_MIN")))
                        (शून्य) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                अन्यथा जर ((t = first_word(ओळ, "SYS_GID_MAX")))
                        (शून्य) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        जर (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        जर (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        परत 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        परत 0;
}

स्थिरांक UGIDAllocationRange *acquire_ugid_allocation_range(शून्य) {
#जर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        स्थिर thread_local UGIDAllocationRange defs;
        स्थिर thread_local पूर्णांक initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        जर (initialized == 0)
                initialized = read_login_defs(&defs, रिक्त, रिक्त) < 0 ? -1 : 1;
        जर (initialized < 0)
                परत &default_ugid_allocation_range;

        परत &defs;

#endif
        परत &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        स्थिरांक UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        परत uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        स्थिरांक UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        परत gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        परत uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
