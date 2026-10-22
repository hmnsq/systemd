/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

จำนวนเต็ม audit_session_from_pid(const PidRef *pid, uint32_t *ret_id);
จำนวนเต็ม audit_loginuid_from_pid(const PidRef *pid, uid_t *ret_uid);

static anta_hstha bool audit_session_is_valid(uint32_t id) {
        คืน id > 0 && id != AUDIT_SESSION_INVALID;
}
