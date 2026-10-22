/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು AUDIT_SESSION_INVALID UINT32_MAX

ಪೂರ್ಣಾಂಕ audit_session_from_pid(ಸ್ಥಿರಾಂಕ PidRef *pid, uint32_t *ret_id);
ಪೂರ್ಣಾಂಕ audit_loginuid_from_pid(ಸ್ಥಿರಾಂಕ PidRef *pid, uid_t *ret_uid);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool audit_session_is_valid(uint32_t id) {
        ಹಿಂತಿರುಗಿ id > 0 && id != AUDIT_SESSION_INVALID;
}
