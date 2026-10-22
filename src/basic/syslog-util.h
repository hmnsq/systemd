/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, முழு);
bool log_facility_unshifted_is_valid(முழு faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, முழு);
bool log_level_is_valid(முழு level);

முழு syslog_parse_priority(மாறிலி எழுத்து **p, முழு *priority, bool with_facility);

bool log_namespace_name_valid(மாறிலி எழுத்து *s);
