/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, পূর্ণাঙ্ক);
bool log_facility_unshifted_is_valid(পূর্ণাঙ্ক faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, পূর্ণাঙ্ক);
bool log_level_is_valid(পূর্ণাঙ্ক level);

পূর্ণাঙ্ক syslog_parse_priority(ধ্রুব বর্ণ **p, পূর্ণাঙ্ক *priority, bool with_facility);

bool log_namespace_name_valid(ধ্রুব বর্ণ *s);
