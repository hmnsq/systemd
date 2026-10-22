/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

पूर्णाङ्क audit_session_from_pid(अचर PidRef *pid, uint32_t *ret_id);
पूर्णाङ्क audit_loginuid_from_pid(अचर PidRef *pid, uid_t *ret_uid);

स्थिर अन्तःस्थ bool audit_session_is_valid(uint32_t id) {
        फर्काऊ id > 0 && id != AUDIT_SESSION_INVALID;
}
