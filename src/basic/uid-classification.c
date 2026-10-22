/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਜੇ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "uid-classification.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

ਸਥਿਰ ਅਚਲ UGIDAllocationRange default_ugid_ਵੰਡ_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#ਜੇ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
ਸਥਿਰ ਪੂਰਨ parse_alloc_uid(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *t, uid_t *ret_uid) {
        uid_t uid;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_uid);

        r = parse_uid(t, &uid);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", ਰਸਤਾ, name, t);
        ਜੇ (uid == 0)
                uid = 1;

        *ret_uid = uid;
        ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ read_login_defs(UGIDAllocationRange *ret_defs, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_defs);

#ਜੇ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        UGIDAllocationRange defs;
        ਪੂਰਨ r;

        ਜੇ (!ਰਸਤਾ)
                ਰਸਤਾ = "/etc/login.defs";

        r = chase_and_fopen_unlocked(ਰਸਤਾ, ਜੜ੍ਹ, CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ, "re", ਨਿਰਾ, &f);
        ਜੇ (r == -ENOENT)
                ਜਾਓ defaults;
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to open %s: %m", ਰਸਤਾ);

        defs = default_ugid_ਵੰਡ_range;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                ਅੱਖਰ *t;

                r = read_ਸਤਰ(f, LINE_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "Failed to read %s: %m", ਰਸਤਾ);
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ ((t = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, "SYS_UID_MIN")))
                        (ਖਾਲੀ) parse_alloc_uid(ਰਸਤਾ, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                ਨਹੀਂਤਾਂ ਜੇ ((t = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, "SYS_UID_MAX")))
                        (ਖਾਲੀ) parse_alloc_uid(ਰਸਤਾ, "SYS_UID_MAX", t, &defs.system_uid_max);
                ਨਹੀਂਤਾਂ ਜੇ ((t = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, "SYS_GID_MIN")))
                        (ਖਾਲੀ) parse_alloc_uid(ਰਸਤਾ, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                ਨਹੀਂਤਾਂ ਜੇ ((t = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, "SYS_GID_MAX")))
                        (ਖਾਲੀ) parse_alloc_uid(ਰਸਤਾ, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        ਜੇ (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", ਰਸਤਾ);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        ਜੇ (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", ਰਸਤਾ);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        ਵਾਪਸ 1;
defaults:
#ਜੇ_ਅੰਤ
        *ret_defs = default_ugid_ਵੰਡ_range;
        ਵਾਪਸ 0;
}

ਅਚਲ UGIDAllocationRange *acquire_ugid_ਵੰਡ_range(ਖਾਲੀ) {
#ਜੇ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        ਸਥਿਰ thread_local UGIDAllocationRange defs;
        ਸਥਿਰ thread_local ਪੂਰਨ initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        ਜੇ (initialized == 0)
                initialized = read_login_defs(&defs, ਨਿਰਾ, ਨਿਰਾ) < 0 ? -1 : 1;
        ਜੇ (initialized < 0)
                ਵਾਪਸ &default_ugid_ਵੰਡ_range;

        ਵਾਪਸ &defs;

#ਜੇ_ਅੰਤ
        ਵਾਪਸ &default_ugid_ਵੰਡ_range;
}

bool uid_is_system(uid_t uid) {
        ਅਚਲ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_ਵੰਡ_range());

        ਵਾਪਸ uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ਅਚਲ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_ਵੰਡ_range());

        ਵਾਪਸ gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        ਵਾਪਸ uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
