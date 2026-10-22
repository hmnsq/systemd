/* SPDX-License-Identifier: LGPL-2.1-or-later */

#אם ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#ضمن <threads.h>

#ضمن "chase.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "log.h"
#ضمن "string-util.h"
#endif

#ضمن "uid-classification.h"
#ضمن "user-util.h"

ساكن ثابت UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#אם ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
ساكن صحيح parse_alloc_uid(ثابت حرف *path, ثابت حرف *name, ثابت حرف *t, uid_t *ret_uid) {
        uid_t uid;
        صحيح r;

        أكد(ret_uid);

        r = parse_uid(t, &uid);
        אם (r < 0)
                החזר log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        אם (uid == 0)
                uid = 1;

        *ret_uid = uid;
        החזר 0;
}
#endif

صحيح read_login_defs(UGIDAllocationRange *ret_defs, ثابت حرف *path, ثابت حرف *root) {
        أكد(ret_defs);

#אם ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = عدم;
        UGIDAllocationRange defs;
        صحيح r;

        אם (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, root, CHASE_PREFIX_ROOT, "re", عدم, &f);
        אם (r == -ENOENT)
                اذهب defaults;
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        עבור (;;) {
                _cleanup_free_ حرف *line = عدم;
                حرف *t;

                r = read_line(f, LINE_MAX, &line);
                אם (r < 0)
                        החזר log_debug_errno(r, "Failed to read %s: %m", path);
                אם (r == 0)
                        اكسر;

                אם ((t = first_word(line, "SYS_UID_MIN")))
                        (فراغ) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                אחרת אם ((t = first_word(line, "SYS_UID_MAX")))
                        (فراغ) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                אחרת אם ((t = first_word(line, "SYS_GID_MIN")))
                        (فراغ) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                אחרת אם ((t = first_word(line, "SYS_GID_MAX")))
                        (فراغ) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        אם (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        אם (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        החזר 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        החזר 0;
}

ثابت UGIDAllocationRange *acquire_ugid_allocation_range(فراغ) {
#אם ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        ساكن thread_local UGIDAllocationRange defs;
        ساكن thread_local صحيح initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        אם (initialized == 0)
                initialized = read_login_defs(&defs, عدم, عدم) < 0 ? -1 : 1;
        אם (initialized < 0)
                החזר &default_ugid_allocation_range;

        החזר &defs;

#endif
        החזר &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        ثابت UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        החזר uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ثابت UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        החזר gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        החזר uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
