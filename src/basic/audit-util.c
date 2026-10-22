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

static ціле audit_read_field(const PidRef *pid, const char *field, char **ret) {
        ціле r;

        assert(field);
        assert(ret);

        якщо (!pidref_is_set(pid))
                повернути -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        якщо (detect_container() > 0)
                повернути -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        якщо (r == -ENOENT) {
                якщо (proc_mounted() == 0)
                        повернути -ENOSYS;
                enoent = true;
        } інакше якщо (r < 0)
                повернути r;

        r = pidref_verify(pid);
        якщо (r < 0)
                повернути r;

        якщо (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                повернути -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        повернути 0;
}

ціле audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        ціле r;

        r = audit_read_field(pid, "sessionid", &s);
        якщо (r < 0)
                повернути r;

        uint32_t u;
        r = safe_atou32(s, &u);
        якщо (r < 0)
                повернути r;

        якщо (!audit_session_is_valid(u))
                повернути -ENODATA;

        якщо (ret_id)
                *ret_id = u;

        повернути 0;
}

ціле audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        ціле r;

        r = audit_read_field(pid, "loginuid", &s);
        якщо (r < 0)
                повернути r;

        якщо (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                повернути -ENODATA;

        повернути parse_uid(s, ret_uid);
}
