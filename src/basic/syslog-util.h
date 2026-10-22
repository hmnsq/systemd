/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, पूर्णाङ्क);
bool log_facility_unshifted_is_valid(पूर्णाङ्क faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, पूर्णाङ्क);
bool log_level_is_valid(पूर्णाङ्क level);

पूर्णाङ्क syslog_parse_priority(अचर वर्ण **p, पूर्णाङ्क *priority, bool with_facility);

bool log_namespace_name_valid(अचर वर्ण *s);
