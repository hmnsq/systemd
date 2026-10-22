/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, પૂર્ણાંક);
bool log_facility_unshifted_is_valid(પૂર્ણાંક faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, પૂર્ણાંક);
bool log_level_is_valid(પૂર્ણાંક level);

પૂર્ણાંક syslog_parse_priority(અચલ અક્ષર **p, પૂર્ણાંક *priority, bool with_facility);

bool log_namespace_name_valid(અચલ અક્ષર *s);
