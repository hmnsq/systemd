/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ຈຳນວນເຕັມ);
bool log_facility_unshifted_is_valid(ຈຳນວນເຕັມ faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, ຈຳນວນເຕັມ);
bool log_level_is_valid(ຈຳນວນເຕັມ level);

ຈຳນວນເຕັມ syslog_parse_priority(const var_na **p, ຈຳນວນເຕັມ *priority, bool with_facility);

bool log_namespace_name_valid(const var_na *s);
