/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, صحیح);
bool log_facility_unshifted_is_valid(صحیح faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, صحیح);
bool log_level_is_valid(صحیح level);

صحیح syslog_parse_priority(ثابت نویسه **p, صحیح *priority, bool with_facility);

bool log_namespace_name_valid(ثابت نویسه *s);
