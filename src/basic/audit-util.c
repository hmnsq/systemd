/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "audit-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "virt.h"

ਸਥਿਰ ਪੂਰਨ audit_read_ਖੇਤਰ(ਅਚਲ PidRef *pid, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਖੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ਜੇ (detect_container() > 0)
                ਵਾਪਸ -ENODATA;

        ਅਚਲ ਅੱਖਰ *p = procfs_file_alloca(pid->pid, ਖੇਤਰ);

        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        bool enoent = false;
        r = read_ਭਰਿਆ_virtual_file(p, &s, /* ret_size= */ ਨਿਰਾ);
        ਜੇ (r == -ENOENT) {
                ਜੇ (proc_mounted() == 0)
                        ਵਾਪਸ -ENOSYS;
                enoent = true;
        } ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ਵਾਪਸ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ audit_session_from_pid(ਅਚਲ PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = audit_read_ਖੇਤਰ(pid, "sessionid", &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        uint32_t u;
        r = ਸੁਰੱਖਿਅਤ_atou32(s, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!audit_session_is_valid(u))
                ਵਾਪਸ -ENODATA;

        ਜੇ (ret_id)
                *ret_id = u;

        ਵਾਪਸ 0;
}

ਪੂਰਨ audit_loginuid_from_pid(ਅਚਲ PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = audit_read_ਖੇਤਰ(pid, "loginuid", &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ਵਾਪਸ -ENODATA;

        ਵਾਪਸ parse_uid(s, ret_uid);
}
