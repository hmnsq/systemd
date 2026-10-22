/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, පූර්ණ_සංඛ්‍යා);
bool log_facility_unshifted_is_valid(පූර්ණ_සංඛ්‍යා faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, පූර්ණ_සංඛ්‍යා);
bool log_level_is_valid(පූර්ණ_සංඛ්‍යා level);

පූර්ණ_සංඛ්‍යා syslog_parse_priority(const var_na **p, පූර්ණ_සංඛ්‍යා *priority, bool with_facility);

bool log_namespace_name_valid(const var_na *s);
