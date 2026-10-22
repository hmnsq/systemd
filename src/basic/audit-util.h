/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

ಪೂರ್ಣಾಙ್ಕ audit_session_from_pid(ಧ್ರುವ PidRef *pid, uint32_t *ret_id);
ಪೂರ್ಣಾಙ್ಕ audit_loginuid_from_pid(ಧ್ರುವ PidRef *pid, uid_t *ret_uid);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool audit_session_is_valid(uint32_t id) {
        ಪ್ರತ್ಯಾಗಮ id > 0 && id != AUDIT_SESSION_INVALID;
}
