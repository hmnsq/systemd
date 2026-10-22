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

स्थिर पूर्णांक audit_read_field(स्थिरांक PidRef *pid, स्थिरांक अक्षर *field, अक्षर **ret) {
        पूर्णांक r;

        प्रतिज्ञा(field);
        प्रतिज्ञा(ret);

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        जर (detect_container() > 0)
                परत -ENODATA;

        स्थिरांक अक्षर *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ अक्षर *s = रिक्त;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ रिक्त);
        जर (r == -ENOENT) {
                जर (proc_mounted() == 0)
                        परत -ENOSYS;
                enoent = true;
        } अन्यथा जर (r < 0)
                परत r;

        r = pidref_verify(pid);
        जर (r < 0)
                परत r;

        जर (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                परत -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक audit_session_from_pid(स्थिरांक PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        r = audit_read_field(pid, "sessionid", &s);
        जर (r < 0)
                परत r;

        uint32_t u;
        r = safe_atou32(s, &u);
        जर (r < 0)
                परत r;

        जर (!audit_session_is_valid(u))
                परत -ENODATA;

        जर (ret_id)
                *ret_id = u;

        परत 0;
}

पूर्णांक audit_loginuid_from_pid(स्थिरांक PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        r = audit_read_field(pid, "loginuid", &s);
        जर (r < 0)
                परत r;

        जर (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                परत -ENODATA;

        परत parse_uid(s, ret_uid);
}
