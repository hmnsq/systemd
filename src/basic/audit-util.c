/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "audit-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "user-util.h"
#அந்தர்பாவ "virt.h"

நிலையான முழு audit_read_field(மாறிலி PidRef *pid, மாறிலி எழுத்து *field, எழுத்து **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(field);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        எனில் (detect_container() > 0)
                திரும்பு -ENODATA;

        மாறிலி எழுத்து *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ எழுத்து *s = வெற்று;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ வெற்று);
        எனில் (r == -ENOENT) {
                எனில் (proc_mounted() == 0)
                        திரும்பு -ENOSYS;
                enoent = true;
        } இல்லையெனில் எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                திரும்பு -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு audit_session_from_pid(மாறிலி PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        r = audit_read_field(pid, "sessionid", &s);
        எனில் (r < 0)
                திரும்பு r;

        uint32_t u;
        r = safe_atou32(s, &u);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!audit_session_is_valid(u))
                திரும்பு -ENODATA;

        எனில் (ret_id)
                *ret_id = u;

        திரும்பு 0;
}

முழு audit_loginuid_from_pid(மாறிலி PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        r = audit_read_field(pid, "loginuid", &s);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                திரும்பு -ENODATA;

        திரும்பு parse_uid(s, ret_uid);
}
