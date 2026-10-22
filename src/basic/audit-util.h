/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUDIT_SESSION_INVALID UINT32_MAX

ਪੂਰਨ audit_session_from_pid(ਅਚਲ PidRef *pid, uint32_t *ret_id);
ਪੂਰਨ audit_loginuid_from_pid(ਅਚਲ PidRef *pid, uid_t *ret_uid);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool audit_session_is_valid(uint32_t id) {
        ਵਾਪਸ id > 0 && id != AUDIT_SESSION_INVALID;
}
