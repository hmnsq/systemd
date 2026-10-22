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

static 整数 audit_read_field(const PidRef *pid, const char *field, char **ret) {
        整数 r;

        assert(field);
        assert(ret);

        如果 (!pidref_is_set(pid))
                返回 -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        如果 (detect_container() > 0)
                返回 -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        如果 (r == -ENOENT) {
                如果 (proc_mounted() == 0)
                        返回 -ENOSYS;
                enoent = true;
        } 否则 如果 (r < 0)
                返回 r;

        r = pidref_verify(pid);
        如果 (r < 0)
                返回 r;

        如果 (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                返回 -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        r = audit_read_field(pid, "sessionid", &s);
        如果 (r < 0)
                返回 r;

        uint32_t u;
        r = safe_atou32(s, &u);
        如果 (r < 0)
                返回 r;

        如果 (!audit_session_is_valid(u))
                返回 -ENODATA;

        如果 (ret_id)
                *ret_id = u;

        返回 0;
}

整数 audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        r = audit_read_field(pid, "loginuid", &s);
        如果 (r < 0)
                返回 r;

        如果 (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                返回 -ENODATA;

        返回 parse_uid(s, ret_uid);
}
