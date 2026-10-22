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

स्थिर पूर्णाङ्क audit_read_क्षेत्रम्(ध्रुव PidRef *pid, ध्रुव वर्ण *क्षेत्रम्, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(क्षेत्रम्);
        प्रतिज्ञा(ret);

        यदि (!pidref_is_समुच्चयः(pid))
                प्रत्यागम -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        यदि (detect_container() > 0)
                प्रत्यागम -ENODATA;

        ध्रुव वर्ण *p = procfs_file_alloca(pid->pid, क्षेत्रम्);

        _cleanup_free_ वर्ण *s = रिक्त;
        bool enoent = false;
        r = read_पूर्णम्_virtual_file(p, &s, /* ret_size= */ रिक्त);
        यदि (r == -ENOENT) {
                यदि (proc_mounted() == 0)
                        प्रत्यागम -ENOSYS;
                enoent = true;
        } अन्यथा यदि (r < 0)
                प्रत्यागम r;

        r = pidref_verify(pid);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                प्रत्यागम -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        प्रत्यागम 0;
}

पूर्णाङ्क audit_session_from_pid(ध्रुव PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_क्षेत्रम्(pid, "sessionid", &s);
        यदि (r < 0)
                प्रत्यागम r;

        uint32_t u;
        r = safe_atou32(s, &u);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!audit_session_is_valid(u))
                प्रत्यागम -ENODATA;

        यदि (ret_id)
                *ret_id = u;

        प्रत्यागम 0;
}

पूर्णाङ्क audit_loginuid_from_pid(ध्रुव PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_क्षेत्रम्(pid, "loginuid", &s);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                प्रत्यागम -ENODATA;

        प्रत्यागम parse_uid(s, ret_uid);
}
