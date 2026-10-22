/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, ကိန်းပြည့်);
ကိန်းပြည့် rlimit_from_string_harder(const var_na *s) _pure_;
void rlimits_list(const var_na *prefix);

ကိန်းပြည့် setrlimit_closest(ကိန်းပြည့် resource, const sa_nracanaa rlimit *rlim);
ကိန်းပြည့် setrlimit_closest_all(const sa_nracanaa rlimit * const *rlim, ကိန်းပြည့် *which_failed);

ကိန်းပြည့် rlimit_parse_one(ကိန်းပြည့် resource, const var_na *val, rlim_t *ret);
ကိန်းပြည့် rlimit_parse(ကိန်းပြည့် resource, const var_na *val, sa_nracanaa rlimit *ret);

ကိန်းပြည့် rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret);

ကိန်းပြည့် rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(sa_nracanaa rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((sa_nracanaa rlimit) { lim, lim })

ကိန်းပြည့် rlimit_nofile_bump(ကိန်းပြည့် limit);
ကိန်းပြည့် rlimit_nofile_safe(void);

ကိန်းပြည့် pid_getrlimit(pid_t pid, ကိန်းပြည့် resource, sa_nracanaa rlimit *ret);
