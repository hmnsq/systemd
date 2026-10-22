/* SPDX-License-Identifier: LGPL-2.1-or-later */

#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#अन्तर्भाव <threads.h>

#अन्तर्भाव "chase.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#यदि_अन्तः

#अन्तर्भाव "uid-classification.h"
#अन्तर्भाव "user-util.h"

स्थिर ध्रुव UGIDAllocationRange default_ugid_आवण्टनम्_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
स्थिर पूर्णाङ्क parse_alloc_uid(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *name, ध्रुव वर्ण *t, uid_t *ret_uid) {
        uid_t uid;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_uid);

        r = parse_uid(t, &uid);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", मार्गः, name, t);
        यदि (uid == 0)
                uid = 1;

        *ret_uid = uid;
        प्रत्यागम 0;
}
#यदि_अन्तः

पूर्णाङ्क read_login_defs(UGIDAllocationRange *ret_defs, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्) {
        प्रतिज्ञा(ret_defs);

#यदि ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = रिक्त;
        UGIDAllocationRange defs;
        पूर्णाङ्क r;

        यदि (!मार्गः)
                मार्गः = "/etc/login.defs";

        r = chase_and_fopen_unlocked(मार्गः, मूलम्, CHASE_उपसर्गः_मूलम्, "re", रिक्त, &f);
        यदि (r == -ENOENT)
                गच्छ defaults;
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to open %s: %m", मार्गः);

        defs = default_ugid_आवण्टनम्_range;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
                वर्ण *t;

                r = read_पङ्क्तिः(f, LINE_MAX, &पङ्क्तिः);
                यदि (r < 0)
                        प्रत्यागम log_debug_errno(r, "Failed to read %s: %m", मार्गः);
                यदि (r == 0)
                        भङ्ग;

                यदि ((t = first_word(पङ्क्तिः, "SYS_UID_MIN")))
                        (शून्य) parse_alloc_uid(मार्गः, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                अन्यथा यदि ((t = first_word(पङ्क्तिः, "SYS_UID_MAX")))
                        (शून्य) parse_alloc_uid(मार्गः, "SYS_UID_MAX", t, &defs.system_uid_max);
                अन्यथा यदि ((t = first_word(पङ्क्तिः, "SYS_GID_MIN")))
                        (शून्य) parse_alloc_uid(मार्गः, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                अन्यथा यदि ((t = first_word(पङ्क्तिः, "SYS_GID_MAX")))
                        (शून्य) parse_alloc_uid(मार्गः, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        यदि (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", मार्गः);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        यदि (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", मार्गः);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        प्रत्यागम 1;
defaults:
#यदि_अन्तः
        *ret_defs = default_ugid_आवण्टनम्_range;
        प्रत्यागम 0;
}

ध्रुव UGIDAllocationRange *acquire_ugid_आवण्टनम्_range(शून्य) {
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
                प्रत्यागम &default_ugid_आवण्टनम्_range;

        प्रत्यागम &defs;

#यदि_अन्तः
        प्रत्यागम &default_ugid_आवण्टनम्_range;
}

bool uid_is_system(uid_t uid) {
        ध्रुव UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_आवण्टनम्_range());

        प्रत्यागम uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ध्रुव UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_आवण्टनम्_range());

        प्रत्यागम gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        प्रत्यागम uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
