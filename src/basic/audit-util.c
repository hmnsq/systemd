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

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        إذا (detect_container() > 0)
                أرجع -ENODATA;

        ثابت حرف *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ حرف *s = عدم;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ عدم);
        إذا (r == -ENOENT) {
                إذا (proc_mounted() == 0)
                        أرجع -ENOSYS;
                enoent = true;
        } وإلا إذا (r < 0)
                أرجع r;

        r = pidref_verify(pid);
        إذا (r < 0)
                أرجع r;

        إذا (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                أرجع -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح audit_session_from_pid(ثابت PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        r = audit_read_field(pid, "sessionid", &s);
        إذا (r < 0)
                أرجع r;

        uint32_t u;
        r = safe_atou32(s, &u);
        إذا (r < 0)
                أرجع r;

        إذا (!audit_session_is_valid(u))
                أرجع -ENODATA;

        إذا (ret_id)
                *ret_id = u;

        أرجع 0;
}

صحيح audit_loginuid_from_pid(ثابت PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        r = audit_read_field(pid, "loginuid", &s);
        إذا (r < 0)
                أرجع r;

        إذا (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                أرجع -ENODATA;

        أرجع parse_uid(s, ret_uid);
}
