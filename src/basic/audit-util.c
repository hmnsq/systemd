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

static цео audit_read_field(const PidRef *pid, const char *field, char **ret) {
        цео r;

        assert(field);
        assert(ret);

        ако (!pidref_is_set(pid))
                врати -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ако (detect_container() > 0)
                врати -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        ако (r == -ENOENT) {
                ако (proc_mounted() == 0)
                        врати -ENOSYS;
                enoent = true;
        } инако ако (r < 0)
                врати r;

        r = pidref_verify(pid);
        ако (r < 0)
                врати r;

        ако (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                врати -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        врати 0;
}

цео audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        цео r;

        r = audit_read_field(pid, "sessionid", &s);
        ако (r < 0)
                врати r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ако (r < 0)
                врати r;

        ако (!audit_session_is_valid(u))
                врати -ENODATA;

        ако (ret_id)
                *ret_id = u;

        врати 0;
}

цео audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        цео r;

        r = audit_read_field(pid, "loginuid", &s);
        ако (r < 0)
                врати r;

        ако (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                врати -ENODATA;

        врати parse_uid(s, ret_uid);
}
