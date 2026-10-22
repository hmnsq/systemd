/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, പൂർണ്ണസംഖ്യ);
bool log_facility_unshifted_is_valid(പൂർണ്ണസംഖ്യ faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, പൂർണ്ണസംഖ്യ);
bool log_level_is_valid(പൂർണ്ണസംഖ്യ level);

പൂർണ്ണസംഖ്യ syslog_parse_priority(സ്ഥിരം അക്ഷരം **p, പൂർണ്ണസംഖ്യ *priority, bool with_facility);

bool log_namespace_name_valid(സ്ഥിരം അക്ഷരം *s);
