/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

പൂർണ്ണസംഖ്യ audit_session_from_pid(സ്ഥിരം PidRef *pid, uint32_t *ret_id);
പൂർണ്ണസംഖ്യ audit_loginuid_from_pid(സ്ഥിരം PidRef *pid, uid_t *ret_uid);

സ്ഥിതം വരിയിൽ bool audit_session_is_valid(uint32_t id) {
        തിരികെ id > 0 && id != AUDIT_SESSION_INVALID;
}
