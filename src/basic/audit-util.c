/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "audit-util.h"
#ضمن "fileio.h"
#ضمن "parse-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "stat-util.h"
#ضمن "user-util.h"
#ضمن "virt.h"

ساكن صحيح audit_read_field(ثابت PidRef *pid, ثابت حرف *field, حرف **ret) {
        صحيح r;

        أكد(field);
        أكد(ret);

        אם (!pidref_is_set(pid))
                החזר -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        אם (detect_container() > 0)
                החזר -ENODATA;

        ثابت حرف *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ حرف *s = عدم;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ عدم);
        אם (r == -ENOENT) {
                אם (proc_mounted() == 0)
                        החזר -ENOSYS;
                enoent = true;
        } אחרת אם (r < 0)
                החזר r;

        r = pidref_verify(pid);
        אם (r < 0)
                החזר r;

        אם (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                החזר -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        החזר 0;
}

صحيح audit_session_from_pid(ثابت PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        r = audit_read_field(pid, "sessionid", &s);
        אם (r < 0)
                החזר r;

        uint32_t u;
        r = safe_atou32(s, &u);
        אם (r < 0)
                החזר r;

        אם (!audit_session_is_valid(u))
                החזר -ENODATA;

        אם (ret_id)
                *ret_id = u;

        החזר 0;
}

صحيح audit_loginuid_from_pid(ثابت PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        r = audit_read_field(pid, "loginuid", &s);
        אם (r < 0)
                החזר r;

        אם (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                החזר -ENODATA;

        החזר parse_uid(s, ret_uid);
}
