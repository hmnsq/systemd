/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు AUDIT_SESSION_INVALID UINT32_MAX

పూర్ణాంకం audit_session_from_pid(స్థిరరాశి PidRef *pid, uint32_t *ret_id);
పూర్ణాంకం audit_loginuid_from_pid(స్థిరరాశి PidRef *pid, uid_t *ret_uid);

స్థిర అంతఃవరుస bool audit_session_is_valid(uint32_t id) {
        తిరిగి id > 0 && id != AUDIT_SESSION_INVALID;
}
