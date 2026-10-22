/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා rlimit_from_string_harder(const var_na *s) _pure_;
void rlimits_list(const var_na *prefix);

පූර්ණ_සංඛ්‍යා setrlimit_closest(පූර්ණ_සංඛ්‍යා resource, const sa_nracanaa rlimit *rlim);
පූර්ණ_සංඛ්‍යා setrlimit_closest_all(const sa_nracanaa rlimit * const *rlim, පූර්ණ_සංඛ්‍යා *which_failed);

පූර්ණ_සංඛ්‍යා rlimit_parse_one(පූර්ණ_සංඛ්‍යා resource, const var_na *val, rlim_t *ret);
පූර්ණ_සංඛ්‍යා rlimit_parse(පූර්ණ_සංඛ්‍යා resource, const var_na *val, sa_nracanaa rlimit *ret);

පූර්ණ_සංඛ්‍යා rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret);

පූර්ණ_සංඛ්‍යා rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(sa_nracanaa rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((sa_nracanaa rlimit) { lim, lim })

පූර්ණ_සංඛ්‍යා rlimit_nofile_bump(පූර්ණ_සංඛ්‍යා limit);
පූර්ණ_සංඛ්‍යා rlimit_nofile_safe(void);

පූර්ණ_සංඛ්‍යා pid_getrlimit(pid_t pid, පූර්ණ_සංඛ්‍යා resource, sa_nracanaa rlimit *ret);
