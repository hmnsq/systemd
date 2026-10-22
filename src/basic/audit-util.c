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

static ကိန်းပြည့် audit_read_field(const PidRef *pid, const var_na *field, var_na **ret) {
        ကိန်းပြည့် r;

        assert(field);
        assert(ret);

        အကယ်၍ (!pidref_is_set(pid))
                ပြန်ပေး -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        အကယ်၍ (detect_container() > 0)
                ပြန်ပေး -ENODATA;

        const var_na *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ var_na *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        အကယ်၍ (r == -ENOENT) {
                အကယ်၍ (proc_mounted() == 0)
                        ပြန်ပေး -ENOSYS;
                enoent = true;
        } မဟုတ်ပါက အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = pidref_verify(pid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ပြန်ပေး -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ပြန်ပေး 0;
}

ကိန်းပြည့် audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ var_na *s = NULL;
        ကိန်းပြည့် r;

        r = audit_read_field(pid, "sessionid", &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        uint32_t u;
        r = safe_atou32(s, &u);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!audit_session_is_valid(u))
                ပြန်ပေး -ENODATA;

        အကယ်၍ (ret_id)
                *ret_id = u;

        ပြန်ပေး 0;
}

ကိန်းပြည့် audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *s = NULL;
        ကိန်းပြည့် r;

        r = audit_read_field(pid, "loginuid", &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ပြန်ပေး -ENODATA;

        ပြန်ပေး parse_uid(s, ret_uid);
}
