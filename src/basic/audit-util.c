/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "audit-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "pidref.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "user-util.h"
#অন্তর্ভাৱ "virt.h"

স্থির পূর্ণাঙ্ক audit_read_field(ধ্রুৱ PidRef *pid, ধ্রুৱ ৱর্ণ *field, ৱর্ণ **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(field);
        প্রতিজ্ঞা(ret);

        যদি (!pidref_is_set(pid))
                ঘূৰাই -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        যদি (detect_container() > 0)
                ঘূৰাই -ENODATA;

        ধ্রুৱ ৱর্ণ *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ রিক্ত);
        যদি (r == -ENOENT) {
                যদি (proc_mounted() == 0)
                        ঘূৰাই -ENOSYS;
                enoent = true;
        } অন্যথা যদি (r < 0)
                ঘূৰাই r;

        r = pidref_verify(pid);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ঘূৰাই -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক audit_session_from_pid(ধ্রুৱ PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        r = audit_read_field(pid, "sessionid", &s);
        যদি (r < 0)
                ঘূৰাই r;

        uint32_t u;
        r = safe_atou32(s, &u);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (!audit_session_is_valid(u))
                ঘূৰাই -ENODATA;

        যদি (ret_id)
                *ret_id = u;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক audit_loginuid_from_pid(ধ্রুৱ PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        r = audit_read_field(pid, "loginuid", &s);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ঘূৰাই -ENODATA;

        ঘূৰাই parse_uid(s, ret_uid);
}
