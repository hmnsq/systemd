/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

পূর্ণসংখ্যা audit_session_from_pid(ধ্রুবক PidRef *pid, uint32_t *ret_id);
পূর্ণসংখ্যা audit_loginuid_from_pid(ধ্রুবক PidRef *pid, uid_t *ret_uid);

স্থির অন্তঃরেখ bool audit_session_is_valid(uint32_t id) {
        ফেরত id > 0 && id != AUDIT_SESSION_INVALID;
}
