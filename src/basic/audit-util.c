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

static цяло audit_read_field(const PidRef *pid, const char *field, char **ret) {
        цяло r;

        assert(field);
        assert(ret);

        ако (!pidref_is_set(pid))
                върни -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ако (detect_container() > 0)
                върни -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        ако (r == -ENOENT) {
                ако (proc_mounted() == 0)
                        върни -ENOSYS;
                enoent = true;
        } иначе ако (r < 0)
                върни r;

        r = pidref_verify(pid);
        ако (r < 0)
                върни r;

        ако (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                върни -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        върни 0;
}

цяло audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        цяло r;

        r = audit_read_field(pid, "sessionid", &s);
        ако (r < 0)
                върни r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ако (r < 0)
                върни r;

        ако (!audit_session_is_valid(u))
                върни -ENODATA;

        ако (ret_id)
                *ret_id = u;

        върни 0;
}

цяло audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        цяло r;

        r = audit_read_field(pid, "loginuid", &s);
        ако (r < 0)
                върни r;

        ако (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                върни -ENODATA;

        върни parse_uid(s, ret_uid);
}
