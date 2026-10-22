/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "audit-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "user-util.h"
#અન્તર્ભાવ "virt.h"

સ્થિર પૂર્ણાંક audit_read_field(અચલ PidRef *pid, અચલ અક્ષર *field, અક્ષર **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(field);
        પ્રતિજ્ઞા(ret);

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        જો (detect_container() > 0)
                પાછા -ENODATA;

        અચલ અક્ષર *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ અક્ષર *s = ખાલી;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ ખાલી);
        જો (r == -ENOENT) {
                જો (proc_mounted() == 0)
                        પાછા -ENOSYS;
                enoent = true;
        } નહીંતર જો (r < 0)
                પાછા r;

        r = pidref_verify(pid);
        જો (r < 0)
                પાછા r;

        જો (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                પાછા -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક audit_session_from_pid(અચલ PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        r = audit_read_field(pid, "sessionid", &s);
        જો (r < 0)
                પાછા r;

        uint32_t u;
        r = safe_atou32(s, &u);
        જો (r < 0)
                પાછા r;

        જો (!audit_session_is_valid(u))
                પાછા -ENODATA;

        જો (ret_id)
                *ret_id = u;

        પાછા 0;
}

પૂર્ણાંક audit_loginuid_from_pid(અચલ PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        r = audit_read_field(pid, "loginuid", &s);
        જો (r < 0)
                પાછા r;

        જો (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                પાછા -ENODATA;

        પાછા parse_uid(s, ret_uid);
}
