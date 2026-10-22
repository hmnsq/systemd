/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

ପୂର୍ଣ୍ଣସଂଖ୍ୟା audit_session_from_pid(ଅଚଳ PidRef *pid, uint32_t *ret_id);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା audit_loginuid_from_pid(ଅଚଳ PidRef *pid, uid_t *ret_uid);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool audit_session_is_valid(uint32_t id) {
        ଫେରନ୍ତୁ id > 0 && id != AUDIT_SESSION_INVALID;
}
