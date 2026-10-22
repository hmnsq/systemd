/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, целое);
bool log_facility_unshifted_is_valid(целое faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, целое);
bool log_level_is_valid(целое level);

целое syslog_parse_priority(const char **p, целое *priority, bool with_facility);

bool log_namespace_name_valid(const char *s);
