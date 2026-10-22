/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ਪੂਰਨ);
bool log_facility_unshifted_is_valid(ਪੂਰਨ faciliy);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(log_level, ਪੂਰਨ);
bool log_level_is_valid(ਪੂਰਨ level);

ਪੂਰਨ syslog_parse_ਤਰਜੀਹ(ਅਚਲ ਅੱਖਰ **p, ਪੂਰਨ *priority, bool with_facility);

bool log_namespace_name_valid(ਅਚਲ ਅੱਖਰ *s);
