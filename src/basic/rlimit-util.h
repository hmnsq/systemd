/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/resource.h>       /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

#నిర్వచించు _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_పట్టిక_LOOKUP(rlimit, పూర్ణాంకం);
పూర్ణాంకం rlimit_from_string_harder(స్థిరరాశి అక్షరం *s) శుద్ధ;
శూన్య rlimits_list(స్థిరరాశి అక్షరం *ఉపసర్గ);

పూర్ణాంకం setrlimit_closest(పూర్ణాంకం resource, స్థిరరాశి నిర్మాణం rlimit *rlim);
పూర్ణాంకం setrlimit_closest_all(స్థిరరాశి నిర్మాణం rlimit * స్థిరరాశి *rlim, పూర్ణాంకం *which_failed);

పూర్ణాంకం rlimit_parse_one(పూర్ణాంకం resource, స్థిరరాశి అక్షరం *val, rlim_t *ret);
పూర్ణాంకం rlimit_parse(పూర్ణాంకం resource, స్థిరరాశి అక్షరం *val, నిర్మాణం rlimit *ret);

పూర్ణాంకం rlimit_format(స్థిరరాశి నిర్మాణం rlimit *rl, అక్షరం **ret);

పూర్ణాంకం rlimit_copy_all(నిర్మాణం rlimit* లక్ష్యం[స్థిర _RLIMIT_MAX], నిర్మాణం rlimit* స్థిరరాశి ఆధారం[స్థిర _RLIMIT_MAX]);
శూన్య rlimit_free_all(నిర్మాణం rlimit **rl);

#నిర్వచించు RLIMIT_MAKE_CONST(lim) ((నిర్మాణం rlimit) { lim, lim })

పూర్ణాంకం rlimit_nofile_bump(పూర్ణాంకం limit);
పూర్ణాంకం rlimit_nofile_సురక్షిత(శూన్య);

పూర్ణాంకం pid_getrlimit(pid_t pid, పూర్ణాంకం resource, నిర్మాణం rlimit *ret);
