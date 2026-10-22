/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(log_facility_unshifted, صحيح);
bool log_facility_unshifted_is_valid(صحيح faciliy);

DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(log_level, صحيح);
bool log_level_is_valid(صحيح level);

صحيح syslog_parse_ترجیح(ثابت حرف **p, صحيح *priority, bool with_facility);

bool log_namespace_name_valid(ثابت حرف *s);
