/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "audit-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "user-util.h"
#अन्तर्भाव "virt.h"

स्थिर पूर्णाङ्क audit_read_field(अचर PidRef *pid, अचर वर्ण *field, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(field);
        प्रतिज्ञा(ret);

        यदि (!pidref_is_set(pid))
                फर्काऊ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        यदि (detect_container() > 0)
                फर्काऊ -ENODATA;

        अचर वर्ण *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ वर्ण *s = रिक्त;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ रिक्त);
        यदि (r == -ENOENT) {
                यदि (proc_mounted() == 0)
                        फर्काऊ -ENOSYS;
                enoent = true;
        } अन्यथा यदि (r < 0)
                फर्काऊ r;

        r = pidref_verify(pid);
        यदि (r < 0)
                फर्काऊ r;

        यदि (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                फर्काऊ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

पूर्णाङ्क audit_session_from_pid(अचर PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_field(pid, "sessionid", &s);
        यदि (r < 0)
                फर्काऊ r;

        uint32_t u;
        r = safe_atou32(s, &u);
        यदि (r < 0)
                फर्काऊ r;

        यदि (!audit_session_is_valid(u))
                फर्काऊ -ENODATA;

        यदि (ret_id)
                *ret_id = u;

        फर्काऊ 0;
}

पूर्णाङ्क audit_loginuid_from_pid(अचर PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_field(pid, "loginuid", &s);
        यदि (r < 0)
                फर्काऊ r;

        यदि (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                फर्काऊ -ENODATA;

        फर्काऊ parse_uid(s, ret_uid);
}
