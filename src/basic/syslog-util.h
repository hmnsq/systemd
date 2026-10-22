/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

DECLARE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(log_facility_unshifted, పూర్ణాంకం);
bool log_facility_unshifted_is_valid(పూర్ణాంకం faciliy);

DECLARE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(log_level, పూర్ణాంకం);
bool log_level_is_valid(పూర్ణాంకం level);

పూర్ణాంకం syslog_parse_ప్రాధాన్యత(స్థిరరాశి అక్షరం **p, పూర్ణాంకం *priority, bool with_facility);

bool log_namespace_name_valid(స్థిరరాశి అక్షరం *s);
