/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "audit-util.h"
#समावेश "fileio.h"
#समावेश "parse-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "stat-util.h"
#समावेश "user-util.h"
#समावेश "virt.h"

अटल पूर्णांक audit_read_field(स्थिर PidRef *pid, स्थिर अक्षर *field, अक्षर **ret) {
        पूर्णांक r;

        निश्चित(field);
        निश्चित(ret);

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        अगर (detect_container() > 0)
                वापस -ENODATA;

        स्थिर अक्षर *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ अक्षर *s = शून्य;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ शून्य);
        अगर (r == -ENOENT) {
                अगर (proc_mounted() == 0)
                        वापस -ENOSYS;
                enoent = true;
        } अन्यथा अगर (r < 0)
                वापस r;

        r = pidref_verify(pid);
        अगर (r < 0)
                वापस r;

        अगर (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                वापस -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक audit_session_from_pid(स्थिर PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        r = audit_read_field(pid, "sessionid", &s);
        अगर (r < 0)
                वापस r;

        uint32_t u;
        r = safe_atou32(s, &u);
        अगर (r < 0)
                वापस r;

        अगर (!audit_session_is_valid(u))
                वापस -ENODATA;

        अगर (ret_id)
                *ret_id = u;

        वापस 0;
}

पूर्णांक audit_loginuid_from_pid(स्थिर PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        r = audit_read_field(pid, "loginuid", &s);
        अगर (r < 0)
                वापस r;

        अगर (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                वापस -ENODATA;

        वापस parse_uid(s, ret_uid);
}
