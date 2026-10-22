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

static ចំនួនគត់ audit_read_field(const PidRef *pid, const var_na *field, var_na **ret) {
        ចំនួនគត់ r;

        assert(field);
        assert(ret);

        ប្រសិនបើ (!pidref_is_set(pid))
                ត្រឡប់ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ប្រសិនបើ (detect_container() > 0)
                ត្រឡប់ -ENODATA;

        const var_na *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ var_na *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        ប្រសិនបើ (r == -ENOENT) {
                ប្រសិនបើ (proc_mounted() == 0)
                        ត្រឡប់ -ENOSYS;
                enoent = true;
        } បើមិនដូច្នេះ ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = pidref_verify(pid);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ត្រឡប់ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ត្រឡប់ 0;
}

ចំនួនគត់ audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ var_na *s = NULL;
        ចំនួនគត់ r;

        r = audit_read_field(pid, "sessionid", &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        uint32_t u;
        r = safe_atou32(s, &u);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!audit_session_is_valid(u))
                ត្រឡប់ -ENODATA;

        ប្រសិនបើ (ret_id)
                *ret_id = u;

        ត្រឡប់ 0;
}

ចំនួនគត់ audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *s = NULL;
        ចំនួនគត់ r;

        r = audit_read_field(pid, "loginuid", &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ត្រឡប់ -ENODATA;

        ត្រឡប់ parse_uid(s, ret_uid);
}
