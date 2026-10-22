/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, পূর্ণসংখ্যা);
bool log_facility_unshifted_is_valid(পূর্ণসংখ্যা faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, পূর্ণসংখ্যা);
bool log_level_is_valid(পূর্ণসংখ্যা level);

পূর্ণসংখ্যা syslog_parse_priority(ধ্রুবক অক্ষর **p, পূর্ণসংখ্যা *priority, bool with_facility);

bool log_namespace_name_valid(ধ্রুবক অক্ষর *s);
