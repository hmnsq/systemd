/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

முழு audit_session_from_pid(மாறிலி PidRef *pid, uint32_t *ret_id);
முழு audit_loginuid_from_pid(மாறிலி PidRef *pid, uid_t *ret_uid);

நிலையான உள்வரி bool audit_session_is_valid(uint32_t id) {
        திரும்பு id > 0 && id != AUDIT_SESSION_INVALID;
}
