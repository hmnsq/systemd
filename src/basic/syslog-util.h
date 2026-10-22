/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, цео);
bool log_facility_unshifted_is_valid(цео faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, цео);
bool log_level_is_valid(цео level);

цео syslog_parse_priority(const char **p, цео *priority, bool with_facility);

bool log_namespace_name_valid(const char *s);
