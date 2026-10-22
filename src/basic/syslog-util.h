/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ចំនួនគត់);
bool log_facility_unshifted_is_valid(ចំនួនគត់ faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, ចំនួនគត់);
bool log_level_is_valid(ចំនួនគត់ level);

ចំនួនគត់ syslog_parse_priority(const var_na **p, ចំនួនគត់ *priority, bool with_facility);

bool log_namespace_name_valid(const var_na *s);
