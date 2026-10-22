/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, puur_naa_nka);
bool log_facility_unshifted_is_valid(puur_naa_nka faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, puur_naa_nka);
bool log_level_is_valid(puur_naa_nka level);

puur_naa_nka syslog_parse_priority(const var_na **p, puur_naa_nka *priority, bool with_facility);

bool log_namespace_name_valid(const var_na *s);
