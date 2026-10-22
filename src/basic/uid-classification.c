/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଯଦି ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#ଅନ୍ତର୍ଭାଵ <threads.h>

#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#endif

#ଅନ୍ତର୍ଭାଵ "uid-classification.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

ସ୍ଥିର ଅଚଳ UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#ଯଦି ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_alloc_uid(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *t, uid_t *ret_uid) {
        uid_t uid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_uid);

        r = parse_uid(t, &uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        ଯଦି (uid == 0)
                uid = 1;

        *ret_uid = uid;
        ଫେରନ୍ତୁ 0;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_login_defs(UGIDAllocationRange *ret_defs, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ) {
        ପ୍ରତିଜ୍ଞା(ret_defs);

#ଯଦି ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ଖାଲି;
        UGIDAllocationRange defs;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, ମୂଳ, CHASE_PREFIX_ମୂଳ, "re", ଖାଲି, &f);
        ଯଦି (r == -ENOENT)
                ଯାଅ defaults;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
                ଅକ୍ଷର *t;

                r = read_ଧାଡ଼ି(f, LINE_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to read %s: %m", path);
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି ((t = first_word(ଧାଡ଼ି, "SYS_UID_MIN")))
                        (ଶୂନ୍ୟ) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                ଅନ୍ୟଥା ଯଦି ((t = first_word(ଧାଡ଼ି, "SYS_UID_MAX")))
                        (ଶୂନ୍ୟ) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                ଅନ୍ୟଥା ଯଦି ((t = first_word(ଧାଡ଼ି, "SYS_GID_MIN")))
                        (ଶୂନ୍ୟ) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                ଅନ୍ୟଥା ଯଦି ((t = first_word(ଧାଡ଼ି, "SYS_GID_MAX")))
                        (ଶୂନ୍ୟ) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        ଯଦି (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        ଯଦି (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        ଫେରନ୍ତୁ 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        ଫେରନ୍ତୁ 0;
}

ଅଚଳ UGIDAllocationRange *acquire_ugid_allocation_range(ଶୂନ୍ୟ) {
#ଯଦି ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        ସ୍ଥିର thread_local UGIDAllocationRange defs;
        ସ୍ଥିର thread_local ପୂର୍ଣ୍ଣସଂଖ୍ୟା initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        ଯଦି (initialized == 0)
                initialized = read_login_defs(&defs, ଖାଲି, ଖାଲି) < 0 ? -1 : 1;
        ଯଦି (initialized < 0)
                ଫେରନ୍ତୁ &default_ugid_allocation_range;

        ଫେରନ୍ତୁ &defs;

#endif
        ଫେରନ୍ତୁ &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        ଅଚଳ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        ଫେରନ୍ତୁ uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        ଅଚଳ UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        ଫେରନ୍ତୁ gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        ଫେରନ୍ତୁ uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
