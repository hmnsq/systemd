/* SPDX-License-Identifier: LGPL-2.1-or-later */

#اگر ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#شامل <threads.h>

#شامل "chase.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "string-util.h"
#endif

#شامل "uid-classification.h"
#شامل "user-util.h"

ایستا ثابت UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#اگر ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
ایستا صحیح parse_alloc_uid(ثابت نویسه *path, ثابت نویسه *name, ثابت نویسه *t, uid_t *ret_uid) {
        uid_t uid;
        صحیح r;

        أكد(ret_uid);

        r = parse_uid(t, &uid);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        اگر (uid == 0)
                uid = 1;

        *ret_uid = uid;
        بازگشت 0;
}
#endif

صحیح read_login_defs(UGIDAllocationRange *ret_defs, ثابت نویسه *path, ثابت نویسه *ریشه) {
        أكد(ret_defs);

#اگر ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = پوچ;
        UGIDAllocationRange defs;
        صحیح r;

        اگر (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, ریشه, CHASE_PREFIX_ریشه, "re", پوچ, &f);
        اگر (r == -ENOENT)
                برو defaults;
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ;
                نویسه *t;

                r = read_سطر(f, LINE_MAX, &سطر);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "Failed to read %s: %m", path);
                اگر (r == 0)
                        بشکن;

                اگر ((t = first_word(سطر, "SYS_UID_MIN")))
                        (تهی) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                وگرنه اگر ((t = first_word(سطر, "SYS_UID_MAX")))
                        (تهی) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                وگرنه اگر ((t = first_word(سطر, "SYS_GID_MIN")))
                        (تهی) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                وگرنه اگر ((t = first_word(سطر, "SYS_GID_MAX")))
                        (تهی) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        اگر (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        اگر (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        بازگشت 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        بازگشت 0;
}

ثابت UGIDAllocationRange *acquire_ugid_allocation_range(تهی) {
#اگر ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        ایستا thread_local UGIDAllocationRange defs;
        ایستا thread_local صحیح initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        اگر (initialized == 0)
                initialized = read_login_defs(&defs, پوچ, پوچ) < 0 ? -1 : 1;
        اگر (initialized < 0)
                بازگشت &default_ugid_allocation_range;

        بازگشت &defs;

#endif
        بازگشت &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        ثابت UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        بازگشت uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ثابت UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        بازگشت gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        بازگشت uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
