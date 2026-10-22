/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ကိန်းပြည့်);
bool log_facility_unshifted_is_valid(ကိန်းပြည့် faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, ကိန်းပြည့်);
bool log_level_is_valid(ကိန်းပြည့် level);

ကိန်းပြည့် syslog_parse_priority(const var_na **p, ကိန်းပြည့် *priority, bool with_facility);

bool log_namespace_name_valid(const var_na *s);
