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

static จำนวนเต็ม audit_read_field(const PidRef *pid, const var_na *field, var_na **ret) {
        จำนวนเต็ม r;

        assert(field);
        assert(ret);

        ถ้า (!pidref_is_set(pid))
                คืน -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ถ้า (detect_container() > 0)
                คืน -ENODATA;

        const var_na *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ var_na *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        ถ้า (r == -ENOENT) {
                ถ้า (proc_mounted() == 0)
                        คืน -ENOSYS;
                enoent = true;
        } มิฉะนั้น ถ้า (r < 0)
                คืน r;

        r = pidref_verify(pid);
        ถ้า (r < 0)
                คืน r;

        ถ้า (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                คืน -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        คืน 0;
}

จำนวนเต็ม audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r;

        r = audit_read_field(pid, "sessionid", &s);
        ถ้า (r < 0)
                คืน r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!audit_session_is_valid(u))
                คืน -ENODATA;

        ถ้า (ret_id)
                *ret_id = u;

        คืน 0;
}

จำนวนเต็ม audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r;

        r = audit_read_field(pid, "loginuid", &s);
        ถ้า (r < 0)
                คืน r;

        ถ้า (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                คืน -ENODATA;

        คืน parse_uid(s, ret_uid);
}
