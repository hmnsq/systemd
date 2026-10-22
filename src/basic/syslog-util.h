/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ಪೂರ್ಣಾಙ್ಕ);
bool log_facility_unshifted_is_valid(ಪೂರ್ಣಾಙ್ಕ faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, ಪೂರ್ಣಾಙ್ಕ);
bool log_level_is_valid(ಪೂರ್ಣಾಙ್ಕ level);

ಪೂರ್ಣಾಙ್ಕ syslog_parse_priority(ಧ್ರುವ ವರ್ಣ **p, ಪೂರ್ಣಾಙ್ಕ *priority, bool with_facility);

bool log_namespace_name_valid(ಧ್ರುವ ವರ್ಣ *s);
