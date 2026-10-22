/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, 정수);
bool log_facility_unshifted_is_valid(정수 faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, 정수);
bool log_level_is_valid(정수 level);

정수 syslog_parse_priority(const char **p, 정수 *priority, bool with_facility);

bool log_namespace_name_valid(const char *s);
