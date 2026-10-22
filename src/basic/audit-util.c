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

static පූර්ණ_සංඛ්‍යා audit_read_field(const PidRef *pid, const var_na *field, var_na **ret) {
        පූර්ණ_සංඛ්‍යා r;

        assert(field);
        assert(ret);

        නම් (!pidref_is_set(pid))
                ආපසු -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        නම් (detect_container() > 0)
                ආපසු -ENODATA;

        const var_na *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ var_na *s = NULL;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ NULL);
        නම් (r == -ENOENT) {
                නම් (proc_mounted() == 0)
                        ආපසු -ENOSYS;
                enoent = true;
        } නැතහොත් නම් (r < 0)
                ආපසු r;

        r = pidref_verify(pid);
        නම් (r < 0)
                ආපසු r;

        නම් (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ආපසු -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා audit_session_from_pid(const PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ var_na *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        r = audit_read_field(pid, "sessionid", &s);
        නම් (r < 0)
                ආපසු r;

        uint32_t u;
        r = safe_atou32(s, &u);
        නම් (r < 0)
                ආපසු r;

        නම් (!audit_session_is_valid(u))
                ආපසු -ENODATA;

        නම් (ret_id)
                *ret_id = u;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        r = audit_read_field(pid, "loginuid", &s);
        නම් (r < 0)
                ආපසු r;

        නම් (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ආපසු -ENODATA;

        ආපසු parse_uid(s, ret_uid);
}
