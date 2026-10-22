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

static 정수 audit_read_field(const PidRef *pid, const char *field, char **ret) {
        정수 r;

        assert(field);
        assert(ret);

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        만약 (detect_container() > 0)
                반환 -ENODATA;

        const char *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ char *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        만약 (r == -ENOENT) {
                만약 (proc_mounted() == 0)
                        반환 -ENOSYS;
                enoent = true;
        } 아니면 만약 (r < 0)
                반환 r;

        r = pidref_verify(pid);
        만약 (r < 0)
                반환 r;

        만약 (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                반환 -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        반환 0;
}

정수 audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        r = audit_read_field(pid, "sessionid", &s);
        만약 (r < 0)
                반환 r;

        uint32_t u;
        r = safe_atou32(s, &u);
        만약 (r < 0)
                반환 r;

        만약 (!audit_session_is_valid(u))
                반환 -ENODATA;

        만약 (ret_id)
                *ret_id = u;

        반환 0;
}

정수 audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        r = audit_read_field(pid, "loginuid", &s);
        만약 (r < 0)
                반환 r;

        만약 (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                반환 -ENODATA;

        반환 parse_uid(s, ret_uid);
}
