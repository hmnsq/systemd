/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, puur_naa_nka);
puur_naa_nka rlimit_from_string_harder(const var_na *s) _pure_;
void rlimits_list(const var_na *prefix);

puur_naa_nka setrlimit_closest(puur_naa_nka resource, const sa_nracanaa rlimit *rlim);
puur_naa_nka setrlimit_closest_all(const sa_nracanaa rlimit * const *rlim, puur_naa_nka *which_failed);

puur_naa_nka rlimit_parse_one(puur_naa_nka resource, const var_na *val, rlim_t *ret);
puur_naa_nka rlimit_parse(puur_naa_nka resource, const var_na *val, sa_nracanaa rlimit *ret);

puur_naa_nka rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret);

puur_naa_nka rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(sa_nracanaa rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((sa_nracanaa rlimit) { lim, lim })

puur_naa_nka rlimit_nofile_bump(puur_naa_nka limit);
puur_naa_nka rlimit_nofile_safe(void);

puur_naa_nka pid_getrlimit(pid_t pid, puur_naa_nka resource, sa_nracanaa rlimit *ret);
