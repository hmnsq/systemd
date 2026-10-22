/* SPDX-License-Identifier: LGPL-2.1-or-later */

#எனில் ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#அந்தர்பாவ <threads.h>

#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "string-util.h"
#endif

#அந்தர்பாவ "uid-classification.h"
#அந்தர்பாவ "user-util.h"

நிலையான மாறிலி UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#எனில் ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
நிலையான முழு parse_alloc_uid(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, மாறிலி எழுத்து *t, uid_t *ret_uid) {
        uid_t uid;
        முழு r;

        ப்ரதிஜ்ஞா(ret_uid);

        r = parse_uid(t, &uid);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        எனில் (uid == 0)
                uid = 1;

        *ret_uid = uid;
        திரும்பு 0;
}
#endif

முழு read_login_defs(UGIDAllocationRange *ret_defs, மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர்) {
        ப்ரதிஜ்ஞா(ret_defs);

#எனில் ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = வெற்று;
        UGIDAllocationRange defs;
        முழு r;

        எனில் (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, வேர், CHASE_PREFIX_வேர், "re", வெற்று, &f);
        எனில் (r == -ENOENT)
                செல் defaults;
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;
                எழுத்து *t;

                r = read_வரி(f, LINE_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "Failed to read %s: %m", path);
                எனில் (r == 0)
                        நிறுத்து;

                எனில் ((t = first_word(வரி, "SYS_UID_MIN")))
                        (வெறுமை) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                இல்லையெனில் எனில் ((t = first_word(வரி, "SYS_UID_MAX")))
                        (வெறுமை) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                இல்லையெனில் எனில் ((t = first_word(வரி, "SYS_GID_MIN")))
                        (வெறுமை) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                இல்லையெனில் எனில் ((t = first_word(வரி, "SYS_GID_MAX")))
                        (வெறுமை) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        எனில் (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        எனில் (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        திரும்பு 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        திரும்பு 0;
}

மாறிலி UGIDAllocationRange *acquire_ugid_allocation_range(வெறுமை) {
#எனில் ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        நிலையான thread_local UGIDAllocationRange defs;
        நிலையான thread_local முழு initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        எனில் (initialized == 0)
                initialized = read_login_defs(&defs, வெற்று, வெற்று) < 0 ? -1 : 1;
        எனில் (initialized < 0)
                திரும்பு &default_ugid_allocation_range;

        திரும்பு &defs;

#endif
        திரும்பு &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        மாறிலி UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        திரும்பு uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        மாறிலி UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        திரும்பு gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        திரும்பு uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
