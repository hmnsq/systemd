/* SPDX-License-Identifier: LGPL-2.1-or-later */

#যদি ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#অন্তর্ভাৱ <threads.h>

#অন্তর্ভাৱ "chase.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "string-util.h"
#endif

#অন্তর্ভাৱ "uid-classification.h"
#অন্তর্ভাৱ "user-util.h"

স্থির ধ্রুৱ UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#যদি ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
স্থির পূর্ণাঙ্ক parse_alloc_uid(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *t, uid_t *ret_uid) {
        uid_t uid;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_uid);

        r = parse_uid(t, &uid);
        যদি (r < 0)
                ঘূৰাই log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        যদি (uid == 0)
                uid = 1;

        *ret_uid = uid;
        ঘূৰাই 0;
}
#endif

পূর্ণাঙ্ক read_login_defs(UGIDAllocationRange *ret_defs, ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *মূল) {
        প্রতিজ্ঞা(ret_defs);

#যদি ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = রিক্ত;
        UGIDAllocationRange defs;
        পূর্ণাঙ্ক r;

        যদি (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, মূল, CHASE_PREFIX_মূল, "re", রিক্ত, &f);
        যদি (r == -ENOENT)
                গচ্ছ defaults;
        যদি (r < 0)
                ঘূৰাই log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *সাৰি = রিক্ত;
                ৱর্ণ *t;

                r = read_সাৰি(f, LINE_MAX, &সাৰি);
                যদি (r < 0)
                        ঘূৰাই log_debug_errno(r, "Failed to read %s: %m", path);
                যদি (r == 0)
                        ভঙ্গ;

                যদি ((t = first_word(সাৰি, "SYS_UID_MIN")))
                        (শূন্য) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                অন্যথা যদি ((t = first_word(সাৰি, "SYS_UID_MAX")))
                        (শূন্য) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                অন্যথা যদি ((t = first_word(সাৰি, "SYS_GID_MIN")))
                        (শূন্য) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                অন্যথা যদি ((t = first_word(সাৰি, "SYS_GID_MAX")))
                        (শূন্য) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        যদি (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        যদি (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        ঘূৰাই 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        ঘূৰাই 0;
}

ধ্রুৱ UGIDAllocationRange *acquire_ugid_allocation_range(শূন্য) {
#যদি ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        স্থির thread_local UGIDAllocationRange defs;
        স্থির thread_local পূর্ণাঙ্ক initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        যদি (initialized == 0)
                initialized = read_login_defs(&defs, রিক্ত, রিক্ত) < 0 ? -1 : 1;
        যদি (initialized < 0)
                ঘূৰাই &default_ugid_allocation_range;

        ঘূৰাই &defs;

#endif
        ঘূৰাই &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        ধ্রুৱ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        ঘূৰাই uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ধ্রুৱ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        ঘূৰাই gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        ঘূৰাই uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
