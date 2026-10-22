/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو AUDIT_SESSION_INVALID UINT32_MAX

صحیح audit_session_from_pid(ثابت PidRef *pid, uint32_t *ret_id);
صحیح audit_loginuid_from_pid(ثابت PidRef *pid, uid_t *ret_uid);

ساکن اندرونی bool audit_session_is_valid(uint32_t id) {
        واپس id > 0 && id != AUDIT_SESSION_INVALID;
}
