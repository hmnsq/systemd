/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, จำนวนเต็ม);
bool log_facility_unshifted_is_valid(จำนวนเต็ม faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, จำนวนเต็ม);
bool log_level_is_valid(จำนวนเต็ม level);

จำนวนเต็ม syslog_parse_priority(const var_na **p, จำนวนเต็ม *priority, bool with_facility);

bool log_namespace_name_valid(const var_na *s);
