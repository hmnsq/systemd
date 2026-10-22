/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, ចំនួនគត់);
ចំនួនគត់ rlimit_from_string_harder(const var_na *s) _pure_;
void rlimits_list(const var_na *prefix);

ចំនួនគត់ setrlimit_closest(ចំនួនគត់ resource, const sa_nracanaa rlimit *rlim);
ចំនួនគត់ setrlimit_closest_all(const sa_nracanaa rlimit * const *rlim, ចំនួនគត់ *which_failed);

ចំនួនគត់ rlimit_parse_one(ចំនួនគត់ resource, const var_na *val, rlim_t *ret);
ចំនួនគត់ rlimit_parse(ចំនួនគត់ resource, const var_na *val, sa_nracanaa rlimit *ret);

ចំនួនគត់ rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret);

ចំនួនគត់ rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(sa_nracanaa rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((sa_nracanaa rlimit) { lim, lim })

ចំនួនគត់ rlimit_nofile_bump(ចំនួនគត់ limit);
ចំនួនគត់ rlimit_nofile_safe(void);

ចំនួនគត់ pid_getrlimit(pid_t pid, ចំនួនគត់ resource, sa_nracanaa rlimit *ret);
