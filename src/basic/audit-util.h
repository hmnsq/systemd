/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

#تعریف_کرو AUDIT_SESSION_INVALID UINT32_MAX

صحيح audit_session_from_pid(ثابت PidRef *pid, uint32_t *ret_id);
صحيح audit_loginuid_from_pid(ثابت PidRef *pid, uid_t *ret_uid);

ساكن مضمن bool audit_session_is_valid(uint32_t id) {
        أرجع id > 0 && id != AUDIT_SESSION_INVALID;
}
