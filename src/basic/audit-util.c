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

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        सचे (detect_container() > 0)
                पटिनिवत्त -ENODATA;

        ध्रुव वर्ण *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ वर्ण *s = रिक्त;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ रिक्त);
        सचे (r == -ENOENT) {
                सचे (proc_mounted() == 0)
                        पटिनिवत्त -ENOSYS;
                enoent = true;
        } अञ्ञथा सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                पटिनिवत्त -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क audit_session_from_pid(ध्रुव PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_field(pid, "sessionid", &s);
        सचे (r < 0)
                पटिनिवत्त r;

        uint32_t u;
        r = safe_atou32(s, &u);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!audit_session_is_valid(u))
                पटिनिवत्त -ENODATA;

        सचे (ret_id)
                *ret_id = u;

        पटिनिवत्त 0;
}

पूर्णाङ्क audit_loginuid_from_pid(ध्रुव PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        r = audit_read_field(pid, "loginuid", &s);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                पटिनिवत्त -ENODATA;

        पटिनिवत्त parse_uid(s, ret_uid);
}
