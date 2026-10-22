/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "audit-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"
#ಅನ್ತರ್ಭಾವ "virt.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ audit_read_ಕ್ಷೇತ್ರ(ಸ್ಥಿರಾಂಕ PidRef *pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕ್ಷೇತ್ರ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        ಆದರೆ (detect_container() > 0)
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = procfs_file_alloca(pid->pid, ಕ್ಷೇತ್ರ);

        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        bool enoent = false;
        r = read_ತುಂಬಿದ_virtual_file(p, &s, /* ret_size= */ ಖಾಲಿ);
        ಆದರೆ (r == -ENOENT) {
                ಆದರೆ (proc_mounted() == 0)
                        ಹಿಂತಿರುಗಿ -ENOSYS;
                enoent = true;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                ಹಿಂತಿರುಗಿ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ audit_session_from_pid(ಸ್ಥಿರಾಂಕ PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = audit_read_ಕ್ಷೇತ್ರ(pid, "sessionid", &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        uint32_t u;
        r = ಸುರಕ್ಷಿತ_atou32(s, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!audit_session_is_valid(u))
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಆದರೆ (ret_id)
                *ret_id = u;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ audit_loginuid_from_pid(ಸ್ಥಿರಾಂಕ PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = audit_read_ಕ್ಷೇತ್ರ(pid, "loginuid", &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಹಿಂತಿರುಗಿ parse_uid(s, ret_uid);
}
