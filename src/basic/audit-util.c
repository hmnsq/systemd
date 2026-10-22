/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "audit-util.h"
#شامل "fileio.h"
#شامل "parse-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "stat-util.h"
#شامل "user-util.h"
#شامل "virt.h"

ایستا صحیح audit_read_field(ثابت PidRef *pid, ثابت نویسه *field, نویسه **ret) {
        صحیح r;

        أكد(field);
        أكد(ret);

        اگر (!pidref_is_set(pid))
                بازگشت -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        اگر (detect_container() > 0)
                بازگشت -ENODATA;

        ثابت نویسه *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ نویسه *s = پوچ;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ پوچ);
        اگر (r == -ENOENT) {
                اگر (proc_mounted() == 0)
                        بازگشت -ENOSYS;
                enoent = true;
        } وگرنه اگر (r < 0)
                بازگشت r;

        r = pidref_verify(pid);
        اگر (r < 0)
                بازگشت r;

        اگر (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                بازگشت -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح audit_session_from_pid(ثابت PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        r = audit_read_field(pid, "sessionid", &s);
        اگر (r < 0)
                بازگشت r;

        uint32_t u;
        r = safe_atou32(s, &u);
        اگر (r < 0)
                بازگشت r;

        اگر (!audit_session_is_valid(u))
                بازگشت -ENODATA;

        اگر (ret_id)
                *ret_id = u;

        بازگشت 0;
}

صحیح audit_loginuid_from_pid(ثابت PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        r = audit_read_field(pid, "loginuid", &s);
        اگر (r < 0)
                بازگشت r;

        اگر (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                بازگشت -ENODATA;

        بازگشت parse_uid(s, ret_uid);
}
