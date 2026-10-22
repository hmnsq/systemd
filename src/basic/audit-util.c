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

static ཧྲིལ audit_read_field(const PidRef *pid, const var_na *field, var_na **ret) {
        ཧྲིལ r;

        assert(field);
        assert(ret);

        གལ་ཏེ (!pidref_is_set(pid))
                ཕྱིར -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        གལ་ཏེ (detect_container() > 0)
                ཕྱིར -ENODATA;

        const var_na *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ var_na *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        གལ་ཏེ (r == -ENOENT) {
                གལ་ཏེ (proc_mounted() == 0)
                        ཕྱིར -ENOSYS;
                enoent = true;
        } དེ་མིན གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = pidref_verify(pid);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ཕྱིར -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

ཧྲིལ audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ var_na *s = NULL;
        ཧྲིལ r;

        r = audit_read_field(pid, "sessionid", &s);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        uint32_t u;
        r = safe_atou32(s, &u);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (!audit_session_is_valid(u))
                ཕྱིར -ENODATA;

        གལ་ཏེ (ret_id)
                *ret_id = u;

        ཕྱིར 0;
}

ཧྲིལ audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *s = NULL;
        ཧྲིལ r;

        r = audit_read_field(pid, "loginuid", &s);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ཕྱིར -ENODATA;

        ཕྱིར parse_uid(s, ret_uid);
}
