/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define AUDIT_SESSION_INVALID UINT32_MAX

પૂર્ણાંક audit_session_from_pid(અચલ PidRef *pid, uint32_t *ret_id);
પૂર્ણાંક audit_loginuid_from_pid(અચલ PidRef *pid, uid_t *ret_uid);

સ્થિર અંતઃરેખ bool audit_session_is_valid(uint32_t id) {
        પાછા id > 0 && id != AUDIT_SESSION_INVALID;
}
