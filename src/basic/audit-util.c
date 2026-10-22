/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "audit-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "user-util.h"
#അന്തര്ഭാവ "virt.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ audit_read_field(സ്ഥിരം PidRef *pid, സ്ഥിരം അക്ഷരം *field, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(field);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        /* Auditing is currently not virtualized for containers. Let's hence not use the audit session ID or
         * login UID for now, it will be leaked in from the host */
        എങ്കിൽ (detect_container() > 0)
                തിരികെ -ENODATA;

        സ്ഥിരം അക്ഷരം *p = procfs_file_alloca(pid->pid, field);

        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        bool enoent = false;
        r = read_full_virtual_file(p, &s, /* ret_size= */ ശൂന്യ);
        എങ്കിൽ (r == -ENOENT) {
                എങ്കിൽ (proc_mounted() == 0)
                        തിരികെ -ENOSYS;
                enoent = true;
        } അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (enoent) /* We got ENOENT, but /proc/ was mounted and the PID still valid? In that case it appears
                     * auditing is not supported by the kernel. */
                തിരികെ -ENODATA;

        delete_trailing_chars(s, NEWLINE);

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ audit_session_from_pid(സ്ഥിരം PidRef *pid, uint32_t *ret_id) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = audit_read_field(pid, "sessionid", &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        uint32_t u;
        r = safe_atou32(s, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!audit_session_is_valid(u))
                തിരികെ -ENODATA;

        എങ്കിൽ (ret_id)
                *ret_id = u;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ audit_loginuid_from_pid(സ്ഥിരം PidRef *pid, uid_t *ret_uid) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = audit_read_field(pid, "loginuid", &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (streq(s, "4294967295")) /* loginuid as 4294967295 means not part of any session. */
                തിരികെ -ENODATA;

        തിരികെ parse_uid(s, ret_uid);
}
