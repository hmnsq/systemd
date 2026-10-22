/* SPDX-License-Identifier: LGPL-2.1-or-later */

#എങ്കിൽ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
#അന്തര്ഭാവ <threads.h>

#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "string-util.h"
#endif

#അന്തര്ഭാവ "uid-classification.h"
#അന്തര്ഭാവ "user-util.h"

സ്ഥിതം സ്ഥിരം UGIDAllocationRange default_ugid_allocation_range = {
        .system_alloc_uid_min = SYSTEM_ALLOC_UID_MIN,
        .system_uid_max = SYSTEM_UID_MAX,
        .system_alloc_gid_min = SYSTEM_ALLOC_GID_MIN,
        .system_gid_max = SYSTEM_GID_MAX,
};

#എങ്കിൽ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_alloc_uid(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *t, uid_t *ret_uid) {
        uid_t uid;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_uid);

        r = parse_uid(t, &uid);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "%s: failed to parse %s %s, ignoring: %m", path, name, t);
        എങ്കിൽ (uid == 0)
                uid = 1;

        *ret_uid = uid;
        തിരികെ 0;
}
#endif

പൂർണ്ണസംഖ്യ read_login_defs(UGIDAllocationRange *ret_defs, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്) {
        പ്രതിജ്ഞാ(ret_defs);

#എങ്കിൽ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        UGIDAllocationRange defs;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!path)
                path = "/etc/login.defs";

        r = chase_and_fopen_unlocked(path, വേര്, CHASE_PREFIX_വേര്, "re", ശൂന്യ, &f);
        എങ്കിൽ (r == -ENOENT)
                പോകുക defaults;
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to open %s: %m", path);

        defs = default_ugid_allocation_range;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
                അക്ഷരം *t;

                r = read_വരി(f, LINE_MAX, &വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "Failed to read %s: %m", path);
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ ((t = first_word(വരി, "SYS_UID_MIN")))
                        (ശൂന്യം) parse_alloc_uid(path, "SYS_UID_MIN", t, &defs.system_alloc_uid_min);
                അല്ലെങ്കിൽ എങ്കിൽ ((t = first_word(വരി, "SYS_UID_MAX")))
                        (ശൂന്യം) parse_alloc_uid(path, "SYS_UID_MAX", t, &defs.system_uid_max);
                അല്ലെങ്കിൽ എങ്കിൽ ((t = first_word(വരി, "SYS_GID_MIN")))
                        (ശൂന്യം) parse_alloc_uid(path, "SYS_GID_MIN", t, &defs.system_alloc_gid_min);
                അല്ലെങ്കിൽ എങ്കിൽ ((t = first_word(വരി, "SYS_GID_MAX")))
                        (ശൂന്യം) parse_alloc_uid(path, "SYS_GID_MAX", t, &defs.system_gid_max);
        }

        എങ്കിൽ (defs.system_alloc_uid_min > defs.system_uid_max) {
                log_debug("%s: SYS_UID_MIN > SYS_UID_MAX, resetting.", path);
                defs.system_alloc_uid_min = MIN(defs.system_uid_max - 1, (uid_t) SYSTEM_ALLOC_UID_MIN);
                /* Look at sys_uid_max to make sure sys_uid_min..sys_uid_max remains a valid range. */
        }
        എങ്കിൽ (defs.system_alloc_gid_min > defs.system_gid_max) {
                log_debug("%s: SYS_GID_MIN > SYS_GID_MAX, resetting.", path);
                defs.system_alloc_gid_min = MIN(defs.system_gid_max - 1, (gid_t) SYSTEM_ALLOC_GID_MIN);
                /* Look at sys_gid_max to make sure sys_gid_min..sys_gid_max remains a valid range. */
        }

        *ret_defs = defs;
        തിരികെ 1;
defaults:
#endif
        *ret_defs = default_ugid_allocation_range;
        തിരികെ 0;
}

സ്ഥിരം UGIDAllocationRange *acquire_ugid_allocation_range(ശൂന്യം) {
#എങ്കിൽ ENABLE_COMPAT_MUTABLE_UID_BOUNDARIES
        സ്ഥിതം thread_local UGIDAllocationRange defs;
        സ്ഥിതം thread_local പൂർണ്ണസംഖ്യ initialized = 0; /* == 0 → not initialized yet
                                                  * < 0 → failure
                                                  * > 0 → success */

        /* This function will ignore failure to read the file, so it should only be called from places where
         * we don't crucially depend on the answer. In other words, it's appropriate for journald, but
         * probably not for sysusers. */

        എങ്കിൽ (initialized == 0)
                initialized = read_login_defs(&defs, ശൂന്യ, ശൂന്യ) < 0 ? -1 : 1;
        എങ്കിൽ (initialized < 0)
                തിരികെ &default_ugid_allocation_range;

        തിരികെ &defs;

#endif
        തിരികെ &default_ugid_allocation_range;
}

bool uid_is_system(uid_t uid) {
        സ്ഥിരം UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        തിരികെ uid <= defs->system_uid_max;
}

bool gid_is_system(gid_t gid) {
        സ്ഥിരം UGIDAllocationRange *defs;
        assert_se(defs = acquire_ugid_allocation_range());

        തിരികെ gid <= defs->system_gid_max;
}

bool uid_for_system_journal(uid_t uid) {

        /* Returns true if the specified UID shall get its data stored in the system journal. */

        തിരികെ uid_is_system(uid) || uid_is_dynamic(uid) || uid_is_greeter(uid) || uid == UID_NOBODY || uid_is_container(uid) || uid_is_foreign(uid);
}
