/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अगर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#समावेश <threads.h>

#समावेश "chase.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "log.h"
#समावेश "string-util.h"
#पूर्ण_अगर

#समावेश "uid-classification.h"
#समावेश "user-util.h"

अटल स्थिर UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#अगर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
अटल पूर्णांक parse_alloc_uid(स्थिर अक्षर *path, स्थिर अक्षर *name, स्थिर अक्षर *t, uid_t *ret_uid) {
        uid_t uid;
        पूर्णांक r;

        निश्चित(ret_uid);

        r = parse_uid(t, &uid);
        अगर (r < 0)
                वापस log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        अगर (uid == 0)
                uid = 1;

        *ret_uid = uid;
        वापस 0;
}
#पूर्ण_अगर

पूर्णांक read_login_defs(UGIDAllocationRange *ret_defs, स्थिर अक्षर *path, स्थिर अक्षर *मूल) {
        निश्चित(ret_defs);

#अगर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ खाता *f = शून्य;
        UGIDAllocationRange defs;
        पूर्णांक r;

        अगर (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, मूल, CHASE_PREFIX_मूल, "re", शून्य, &f);
        अगर (r == -ENOENT)
                जाओ defaults;
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;
                अक्षर *t;

                r = read_पंक्ति(f, LINE_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस log_debug_errno(r, "Failed to read %s: %m", path);
                अगर (r == 0)
                        अवरोध;

                अगर ((t = first_word(पंक्ति, "SYS_UID_MIN")))
                        (व्योम) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                अन्यथा अगर ((t = first_word(पंक्ति, "SYS_UID_MAX")))
                        (व्योम) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                अन्यथा अगर ((t = first_word(पंक्ति, "SYS_GID_MIN")))
                        (व्योम) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                अन्यथा अगर ((t = first_word(पंक्ति, "SYS_GID_MAX")))
                        (व्योम) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        अगर (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        अगर (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        वापस 1;
defaults:
#पूर्ण_अगर
        *ret_defs = default_ugid_allocation_range;
        वापस 0;
}

स्थिर UGIDAllocationRange *acquire_ugid_allocation_range(व्योम) {
#अगर ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        अटल thread_local UGIDAllocationRange defs;
        अटल thread_local पूर्णांक initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        अगर (initialized == 0)
                initialized = read_login_defs(&defs, शून्य, शून्य) < 0 ? -1 : 1;
        अगर (initialized < 0)
                वापस &default_ugid_allocation_range;

        वापस &defs;

#पूर्ण_अगर
        वापस &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        स्थिर UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        वापस uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        स्थिर UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        वापस gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        वापस uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
