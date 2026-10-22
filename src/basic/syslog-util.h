/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, 整数);
bool log_facility_unshifted_is_valid(整数 faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, 整数);
bool log_level_is_valid(整数 level);

整数 syslog_parse_priority(const char **p, 整数 *priority, bool with_facility);

bool log_namespace_name_valid(const char *s);
