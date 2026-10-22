/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "audit-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "user-util.h"
#অন্তর্ভাব "virt.h"

স্থির পূর্ণসংখ্যা audit_read_field(ধ্রুবক PidRef *pid, ধ্রুবক অক্ষর *field, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(field);
        প্রতিজ্ঞা(ret);

        যদি (!pidref_is_set(pid))
                ফেরত -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        যদি (detect_container() > 0)
                ফেরত -ENODATA;

        ধ্রুবক অক্ষর *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ অক্ষর *s = ফাঁকা;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ ফাঁকা);
        যদি (r == -ENOENT) {
                যদি (proc_mounted() == 0)
                        ফেরত -ENOSYS;
                enoent = true;
        } অন্যথা যদি (r < 0)
                ফেরত r;

        r = pidref_verify(pid);
        যদি (r < 0)
                ফেরত r;

        যদি (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ফেরত -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা audit_session_from_pid(ধ্রুবক PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = audit_read_field(pid, "sessionid", &s);
        যদি (r < 0)
                ফেরত r;

        uint32_t u;
        r = safe_atou32(s, &u);
        যদি (r < 0)
                ফেরত r;

        যদি (!audit_session_is_valid(u))
                ফেরত -ENODATA;

        যদি (ret_id)
                *ret_id = u;

        ফেরত 0;
}

পূর্ণসংখ্যা audit_loginuid_from_pid(ধ্রুবক PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = audit_read_field(pid, "loginuid", &s);
        যদি (r < 0)
                ফেরত r;

        যদি (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ফেরত -ENODATA;

        ফেরত parse_uid(s, ret_uid);
}
