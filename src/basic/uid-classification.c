/* SPDX-License-Identifier: LGPL-2.1-or-later */

#якщо ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#include <threads.h>

#include "chase.h"
#include "fd-util.h"
#include "fileio.h"
#include "log.h"
#include "string-util.h"
#endif

#include "uid-classification.h"
#include "user-util.h"

static const UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#якщо ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
static ціле parse_alloc_uid(const char *path, const char *name, const char *t, uid_t *ret_uid) {
        uid_t uid;
        ціле r;

        assert(ret_uid);

        r = parse_uid(t, &uid);
        якщо (r < 0)
                повернути log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        якщо (uid == 0)
                uid = 1;

        *ret_uid = uid;
        повернути 0;
}
#endif

ціле read_login_defs(UGIDAllocationRange *ret_defs, const char *path, const char *root) {
        assert(ret_defs);

#якщо ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = NULL;
        UGIDAllocationRange defs;
        ціле r;

        якщо (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, root, CHASE_PREFIX_ROOT, "re", NULL, &f);
        якщо (r == -ENOENT)
                goto defaults;
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        для (;;) {
                _cleanup_free_ char *line = NULL;
                char *t;

                r = read_line(f, LINE_MAX, &line);
                якщо (r < 0)
                        повернути log_debug_errno(r, "Failed to read %s: %m", path);
                якщо (r == 0)
                        break;

                якщо ((t = first_word(line, "SYS_UID_MIN")))
                        (void) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                інакше якщо ((t = first_word(line, "SYS_UID_MAX")))
                        (void) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                інакше якщо ((t = first_word(line, "SYS_GID_MIN")))
                        (void) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                інакше якщо ((t = first_word(line, "SYS_GID_MAX")))
                        (void) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        якщо (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        якщо (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        повернути 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        повернути 0;
}

const UGIDAllocationRange *acquire_ugid_allocation_range(void) {
#якщо ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        static thread_local UGIDAllocationRange defs;
        static thread_local ціле initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        якщо (initialized == 0)
                initialized = read_login_defs(&defs, NULL, NULL) < 0 ? -1 : 1;
        якщо (initialized < 0)
                повернути &default_ugid_allocation_range;

        повернути &defs;

#endif
        повернути &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        const UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        повернути uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        const UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        повернути gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        повернути uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
