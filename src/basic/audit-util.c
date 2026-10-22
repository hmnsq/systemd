/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "audit-util.h"
#include "fileio.h"
#include "parse-util.h"
#include "pidref.h"
#include "process-util.h"
#include "stat-util.h"
#include "user-util.h"
#include "virt.h"

static puur_naa_nka audit_read_field(const PidRef *pid, const var_na *field, var_na **ret) {
        puur_naa_nka r;

        assert(field);
        assert(ret);

        ᰀᰧ (!pidref_is_set(pid))
                ᰀᰧᰌ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ᰀᰧ (detect_container() > 0)
                ᰀᰧᰌ -ENODATA;

        const var_na *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ var_na *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        ᰀᰧ (r == -ENOENT) {
                ᰀᰧ (proc_mounted() == 0)
                        ᰀᰧᰌ -ENOSYS;
                enoent = true;
        } ᰀᰧᰀ ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        r = pidref_verify(pid);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ᰀᰧᰌ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 0;
}

puur_naa_nka audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        r = audit_read_field(pid, "sessionid", &s);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (!audit_session_is_valid(u))
                ᰀᰧᰌ -ENODATA;

        ᰀᰧ (ret_id)
                *ret_id = u;

        ᰀᰧᰌ 0;
}

puur_naa_nka audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        r = audit_read_field(pid, "loginuid", &s);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ᰀᰧᰌ -ENODATA;

        ᰀᰧᰌ parse_uid(s, ret_uid);
}
