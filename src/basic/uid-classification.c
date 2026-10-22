/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ఒకవేళ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#అన్తర్భావ <threads.h>

#అన్తర్భావ "chase.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "string-util.h"
#అయితే_ముగింపు

#అన్తర్భావ "uid-classification.h"
#అన్తర్భావ "user-util.h"

స్థిర స్థిరరాశి UGIDAllocationRange default_ugid_కేటాయింపు_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#ఒకవేళ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
స్థిర పూర్ణాంకం parse_alloc_uid(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *t, uid_t *ret_uid) {
        uid_t uid;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_uid);

        r = parse_uid(t, &uid);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", మార్గం, name, t);
        ఒకవేళ (uid == 0)
                uid = 1;

        *ret_uid = uid;
        తిరిగి 0;
}
#అయితే_ముగింపు

పూర్ణాంకం read_login_defs(UGIDAllocationRange *ret_defs, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం) {
        ప్రతిజ్ఞా(ret_defs);

#ఒకవేళ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ఖాళీ;
        UGIDAllocationRange defs;
        పూర్ణాంకం r;

        ఒకవేళ (!మార్గం)
                మార్గం = "/etc/login.defs";

        r = chase_and_fopen_unlocked(మార్గం, మూలం, CHASE_ఉపసర్గ_మూలం, "re", ఖాళీ, &f);
        ఒకవేళ (r == -ENOENT)
                వెళ్ళు defaults;
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to open %s: %m", మార్గం);

        defs = default_ugid_కేటాయింపు_range;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                అక్షరం *t;

                r = read_పంక్తి(f, LINE_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "Failed to read %s: %m", మార్గం);
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ ((t = మొదటి_మాట(పంక్తి, "SYS_UID_MIN")))
                        (శూన్య) parse_alloc_uid(మార్గం, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                లేకపోతే ఒకవేళ ((t = మొదటి_మాట(పంక్తి, "SYS_UID_MAX")))
                        (శూన్య) parse_alloc_uid(మార్గం, "SYS_UID_MAX", t, &defs.system_uid_max);
                లేకపోతే ఒకవేళ ((t = మొదటి_మాట(పంక్తి, "SYS_GID_MIN")))
                        (శూన్య) parse_alloc_uid(మార్గం, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                లేకపోతే ఒకవేళ ((t = మొదటి_మాట(పంక్తి, "SYS_GID_MAX")))
                        (శూన్య) parse_alloc_uid(మార్గం, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        ఒకవేళ (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", మార్గం);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        ఒకవేళ (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", మార్గం);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        తిరిగి 1;
defaults:
#అయితే_ముగింపు
        *ret_defs = default_ugid_కేటాయింపు_range;
        తిరిగి 0;
}

స్థిరరాశి UGIDAllocationRange *acquire_ugid_కేటాయింపు_range(శూన్య) {
#ఒకవేళ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        స్థిర thread_local UGIDAllocationRange defs;
        స్థిర thread_local పూర్ణాంకం initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        ఒకవేళ (initialized == 0)
                initialized = read_login_defs(&defs, ఖాళీ, ఖాళీ) < 0 ? -1 : 1;
        ఒకవేళ (initialized < 0)
                తిరిగి &default_ugid_కేటాయింపు_range;

        తిరిగి &defs;

#అయితే_ముగింపు
        తిరిగి &default_ugid_కేటాయింపు_range;
}

bool uid_is_system(uid_t uid) {
        స్థిరరాశి UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_కేటాయింపు_range());

        తిరిగి uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        స్థిరరాశి UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_కేటాయింపు_range());

        తిరిగి gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        తిరిగి uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
