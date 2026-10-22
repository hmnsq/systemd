/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "audit-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"
#ಅನ್ತರ್ಭಾವ "virt.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ audit_read_field(ಧ್ರುವ PidRef *pid, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(field);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ಯದಿ (detect_container() > 0)
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಧ್ರುವ ವರ್ಣ *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ ರಿಕ್ತ);
        ಯದಿ (r == -ENOENT) {
                ಯದಿ (proc_mounted() == 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOSYS;
                enoent = true;
        } ಅನ್ಯಥಾ ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ audit_session_from_pid(ಧ್ರುವ PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = audit_read_field(pid, "sessionid", &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!audit_session_is_valid(u))
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಯದಿ (ret_id)
                *ret_id = u;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ audit_loginuid_from_pid(ಧ್ರುವ PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = audit_read_field(pid, "loginuid", &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಪ್ರತ್ಯಾಗಮ parse_uid(s, ret_uid);
}
