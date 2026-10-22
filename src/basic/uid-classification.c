/* SPDX-License-Identifier: LGPL-2.1-or-later */

#જો ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#અન્તર્ભાવ <threads.h>

#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "string-util.h"
#endif

#અન્તર્ભાવ "uid-classification.h"
#અન્તર્ભાવ "user-util.h"

સ્થિર અચલ UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#જો ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
સ્થિર પૂર્ણાંક parse_alloc_uid(અચલ અક્ષર *path, અચલ અક્ષર *name, અચલ અક્ષર *t, uid_t *ret_uid) {
        uid_t uid;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_uid);

        r = parse_uid(t, &uid);
        જો (r < 0)
                પાછા log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        જો (uid == 0)
                uid = 1;

        *ret_uid = uid;
        પાછા 0;
}
#endif

પૂર્ણાંક read_login_defs(UGIDAllocationRange *ret_defs, અચલ અક્ષર *path, અચલ અક્ષર *મૂળ) {
        પ્રતિજ્ઞા(ret_defs);

#જો ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ખાલી;
        UGIDAllocationRange defs;
        પૂર્ણાંક r;

        જો (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, મૂળ, CHASE_PREFIX_મૂળ, "re", ખાલી, &f);
        જો (r == -ENOENT)
                જાઓ defaults;
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;
                અક્ષર *t;

                r = read_લીટી(f, LINE_MAX, &લીટી);
                જો (r < 0)
                        પાછા log_debug_errno(r, "Failed to read %s: %m", path);
                જો (r == 0)
                        તોડો;

                જો ((t = first_word(લીટી, "SYS_UID_MIN")))
                        (શૂન્ય) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                નહીંતર જો ((t = first_word(લીટી, "SYS_UID_MAX")))
                        (શૂન્ય) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                નહીંતર જો ((t = first_word(લીટી, "SYS_GID_MIN")))
                        (શૂન્ય) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                નહીંતર જો ((t = first_word(લીટી, "SYS_GID_MAX")))
                        (શૂન્ય) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        જો (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        જો (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        પાછા 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        પાછા 0;
}

અચલ UGIDAllocationRange *acquire_ugid_allocation_range(શૂન્ય) {
#જો ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        સ્થિર thread_local UGIDAllocationRange defs;
        સ્થિર thread_local પૂર્ણાંક initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        જો (initialized == 0)
                initialized = read_login_defs(&defs, ખાલી, ખાલી) < 0 ? -1 : 1;
        જો (initialized < 0)
                પાછા &default_ugid_allocation_range;

        પાછા &defs;

#endif
        પાછા &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        અચલ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        પાછા uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        અચલ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        પાછા gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        પાછા uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
