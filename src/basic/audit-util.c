/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "audit-util.h"
#include "fileio.h"
#include "parse-util.h"
#include "pidref.h"
#include "process-util.h"
#include "stat-util.h"
#include "user-util.h"
#include "virt.h"

static số_nguyên audit_read_field(const PidRef *pid, const char *field, char **ret) {
        số_nguyên r;

        assert(field);
        assert(ret);

        nếu (!pidref_is_set(pid))
                trả_về -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        nếu (detect_container() > 0)
                trả_về -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        nếu (r == -ENOENT) {
                nếu (proc_mounted() == 0)
                        trả_về -ENOSYS;
                enoent = true;
        } không_thì nếu (r < 0)
                trả_về r;

        r = pidref_verify(pid);
        nếu (r < 0)
                trả_về r;

        nếu (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                trả_về -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        trả_về 0;
}

số_nguyên audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        r = audit_read_field(pid, "sessionid", &s);
        nếu (r < 0)
                trả_về r;

        uint32_t u;
        r = safe_atou32(s, &u);
        nếu (r < 0)
                trả_về r;

        nếu (!audit_session_is_valid(u))
                trả_về -ENODATA;

        nếu (ret_id)
                *ret_id = u;

        trả_về 0;
}

số_nguyên audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        r = audit_read_field(pid, "loginuid", &s);
        nếu (r < 0)
                trả_về r;

        nếu (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                trả_về -ENODATA;

        trả_về parse_uid(s, ret_uid);
}
