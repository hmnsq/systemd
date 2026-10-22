/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "audit-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"
#ଅନ୍ତର୍ଭାଵ "virt.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା audit_read_field(ଅଚଳ PidRef *pid, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(field);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ଯଦି (detect_container() > 0)
                ଫେରନ୍ତୁ -ENODATA;

        ଅଚଳ ଅକ୍ଷର *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ ଖାଲି);
        ଯଦି (r == -ENOENT) {
                ଯଦି (proc_mounted() == 0)
                        ଫେରନ୍ତୁ -ENOSYS;
                enoent = true;
        } ଅନ୍ୟଥା ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ଫେରନ୍ତୁ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା audit_session_from_pid(ଅଚଳ PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = audit_read_field(pid, "sessionid", &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!audit_session_is_valid(u))
                ଫେରନ୍ତୁ -ENODATA;

        ଯଦି (ret_id)
                *ret_id = u;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା audit_loginuid_from_pid(ଅଚଳ PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = audit_read_field(pid, "loginuid", &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ଫେରନ୍ତୁ -ENODATA;

        ଫେରନ୍ତୁ parse_uid(s, ret_uid);
}
