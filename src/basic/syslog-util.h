/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ಪೂರ್ಣಾಂಕ);
bool log_facility_unshifted_is_valid(ಪೂರ್ಣಾಂಕ faciliy);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(log_level, ಪೂರ್ಣಾಂಕ);
bool log_level_is_valid(ಪೂರ್ಣಾಂಕ level);

ಪೂರ್ಣಾಂಕ syslog_parse_ಆದ್ಯತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಪೂರ್ಣಾಂಕ *priority, bool with_facility);

bool log_namespace_name_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
