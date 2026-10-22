/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

पूर्णांक audit_session_from_pid(स्थिरांक PidRef *pid, uint32_t *ret_id);
पूर्णांक audit_loginuid_from_pid(स्थिरांक PidRef *pid, uid_t *ret_uid);

स्थिर ओळीत bool audit_session_is_valid(uint32_t id) {
        परत id > 0 && id != AUDIT_SESSION_INVALID;
}
