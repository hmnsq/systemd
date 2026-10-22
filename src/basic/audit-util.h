/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा AUDIT_SESSION_INVALID UINT32_MAX

पूर्णांक audit_session_from_pid(स्थिर PidRef *pid, uint32_t *ret_id);
पूर्णांक audit_loginuid_from_pid(स्थिर PidRef *pid, uid_t *ret_uid);

अटल अंतरभूत bool audit_session_is_valid(uint32_t id) {
        वापस id > 0 && id != AUDIT_SESSION_INVALID;
}
