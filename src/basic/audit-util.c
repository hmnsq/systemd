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

ساکن صحیح audit_read_میدان(ثابت PidRef *pid, ثابت حرف *میدان, حرف **ret) {
        صحیح r;

        أكد(میدان);
        أكد(ret);

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        اگر (detect_container() > 0)
                واپس -ENODATA;

        ثابت حرف *p = procfs_file_alloca(pid->pid, میدان);

        _cleanup_free_ حرف *s = تہی;
        bool enoent = false;
        r = read_بھرا_virtual_file(p, &s, /* ret_size= */ تہی);
        اگر (r == -ENOENT) {
                اگر (proc_mounted() == 0)
                        واپس -ENOSYS;
                enoent = true;
        } ورنہ اگر (r < 0)
                واپس r;

        r = pidref_verify(pid);
        اگر (r < 0)
                واپس r;

        اگر (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                واپس -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح audit_session_from_pid(ثابت PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        r = audit_read_میدان(pid, "sessionid", &s);
        اگر (r < 0)
                واپس r;

        uint32_t u;
        r = محفوظ_atou32(s, &u);
        اگر (r < 0)
                واپس r;

        اگر (!audit_session_is_valid(u))
                واپس -ENODATA;

        اگر (ret_id)
                *ret_id = u;

        واپس 0;
}

صحیح audit_loginuid_from_pid(ثابت PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        r = audit_read_میدان(pid, "loginuid", &s);
        اگر (r < 0)
                واپس r;

        اگر (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                واپس -ENODATA;

        واپس parse_uid(s, ret_uid);
}
