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

स्थिर पूर्णाङ्क audit_read_field(ध्रुव PidRef *pid, ध्रुव वर्ण *field, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(field);
        प्रतिज्ञा(ret);

        जइ (!pidref_is_set(pid))
                प्रत्यागम -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        जइ (detect_container() > 0)
                प्रत्यागम -ENODATA;

        ध्रुव वर्ण *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ वर्ण *s = रिक्त;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ रिक्त);
        जइ (r == -ENOENT) {
                जइ (proc_mounted() == 0)
                        प्रत्यागम -ENOSYS;
                enoent = true;
        } अण्णहा जइ (r < 0)
                प्रत्यागम r;

        r = pidref_verify(pid);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                प्रत्यागम -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        प्रत्यागम 0;
}

पूर्णाङ्क audit_session_from_pid(ध्रुव PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_field(pid, "sessionid", &s);
        जइ (r < 0)
                प्रत्यागम r;

        uint32_t u;
        r = safe_atou32(s, &u);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (!audit_session_is_valid(u))
                प्रत्यागम -ENODATA;

        जइ (ret_id)
                *ret_id = u;

        प्रत्यागम 0;
}

पूर्णाङ्क audit_loginuid_from_pid(ध्रुव PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_field(pid, "loginuid", &s);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                प्रत्यागम -ENODATA;

        प्रत्यागम parse_uid(s, ret_uid);
}
