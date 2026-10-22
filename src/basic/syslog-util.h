/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, पूर्णांक);
bool log_facility_unshifted_is_valid(पूर्णांक faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, पूर्णांक);
bool log_level_is_valid(पूर्णांक level);

पूर्णांक syslog_parse_priority(स्थिरांक अक्षर **p, पूर्णांक *priority, bool with_facility);

bool log_namespace_name_valid(स्थिरांक अक्षर *s);
