/* SPDX-License-Identifier: LGPL-2.1-or-later */

#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#अन्तर्भाव <threads.h>

#अन्तर्भाव "chase.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#endif

#अन्तर्भाव "uid-classification.h"
#अन्तर्भाव "user-util.h"

स्थिर ध्रुव UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
स्थिर पूर्णाङ्क parse_alloc_uid(ध्रुव वर्ण *path, ध्रुव वर्ण *name, ध्रुव वर्ण *t, uid_t *ret_uid) {
        uid_t uid;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_uid);

        r = parse_uid(t, &uid);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        यदि (uid == 0)
                uid = 1;

        *ret_uid = uid;
        प्रत्यागम 0;
}
#endif

पूर्णाङ्क read_login_defs(UGIDAllocationRange *ret_defs, ध्रुव वर्ण *path, ध्रुव वर्ण *मूळ) {
        प्रतिज्ञा(ret_defs);

#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = रिक्त;
        UGIDAllocationRange defs;
        पूर्णाङ्क r;

        यदि (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, मूळ, CHASE_PREFIX_मूळ, "re", रिक्त, &f);
        यदि (r == -ENOENT)
                गच्छ defaults;
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        प्रति (;;) {
                _cleanup_free_ वर्ण *ओळ = रिक्त;
                वर्ण *t;

                r = read_ओळ(f, LINE_MAX, &ओळ);
                यदि (r < 0)
                        प्रत्यागम log_debug_errno(r, "Failed to read %s: %m", path);
                यदि (r == 0)
                        भङ्ग;

                यदि ((t = first_word(ओळ, "SYS_UID_MIN")))
                        (शून्य) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                अन्यथा यदि ((t = first_word(ओळ, "SYS_UID_MAX")))
                        (शून्य) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                अन्यथा यदि ((t = first_word(ओळ, "SYS_GID_MIN")))
                        (शून्य) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                अन्यथा यदि ((t = first_word(ओळ, "SYS_GID_MAX")))
                        (शून्य) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        यदि (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        यदि (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        प्रत्यागम 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        प्रत्यागम 0;
}

ध्रुव UGIDAllocationRange *acquire_ugid_allocation_range(शून्य) {
#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        स्थिर thread_local UGIDAllocationRange defs;
        स्थिर thread_local पूर्णाङ्क initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        यदि (initialized == 0)
                initialized = read_login_defs(&defs, रिक्त, रिक्त) < 0 ? -1 : 1;
        यदि (initialized < 0)
                प्रत्यागम &default_ugid_allocation_range;

        प्रत्यागम &defs;

#endif
        प्रत्यागम &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        ध्रुव UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        प्रत्यागम uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ध्रुव UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        प्रत्यागम gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        प्रत्यागम uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
