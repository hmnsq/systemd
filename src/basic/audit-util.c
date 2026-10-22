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

static целое audit_read_field(const PidRef *pid, const char *field, char **ret) {
        целое r;

        assert(field);
        assert(ret);

        если (!pidref_is_set(pid))
                вернуть -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        если (detect_container() > 0)
                вернуть -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        если (r == -ENOENT) {
                если (proc_mounted() == 0)
                        вернуть -ENOSYS;
                enoent = true;
        } иначе если (r < 0)
                вернуть r;

        r = pidref_verify(pid);
        если (r < 0)
                вернуть r;

        если (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                вернуть -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        вернуть 0;
}

целое audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        целое r;

        r = audit_read_field(pid, "sessionid", &s);
        если (r < 0)
                вернуть r;

        uint32_t u;
        r = safe_atou32(s, &u);
        если (r < 0)
                вернуть r;

        если (!audit_session_is_valid(u))
                вернуть -ENODATA;

        если (ret_id)
                *ret_id = u;

        вернуть 0;
}

целое audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        целое r;

        r = audit_read_field(pid, "loginuid", &s);
        если (r < 0)
                вернуть r;

        если (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                вернуть -ENODATA;

        вернуть parse_uid(s, ret_uid);
}
