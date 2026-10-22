/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
bool log_facility_unshifted_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା faciliy);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
bool log_level_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା syslog_parse_priority(ଅଚଳ ଅକ୍ଷର **p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *priority, bool with_facility);

bool log_namespace_name_valid(ଅଚଳ ଅକ୍ଷର *s);
