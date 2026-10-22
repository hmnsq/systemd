/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ rlimit_from_string_harder(const var_na *s) _pure_;
void rlimits_list(const var_na *prefix);

ຈຳນວນເຕັມ setrlimit_closest(ຈຳນວນເຕັມ resource, const sa_nracanaa rlimit *rlim);
ຈຳນວນເຕັມ setrlimit_closest_all(const sa_nracanaa rlimit * const *rlim, ຈຳນວນເຕັມ *which_failed);

ຈຳນວນເຕັມ rlimit_parse_one(ຈຳນວນເຕັມ resource, const var_na *val, rlim_t *ret);
ຈຳນວນເຕັມ rlimit_parse(ຈຳນວນເຕັມ resource, const var_na *val, sa_nracanaa rlimit *ret);

ຈຳນວນເຕັມ rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret);

ຈຳນວນເຕັມ rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(sa_nracanaa rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((sa_nracanaa rlimit) { lim, lim })

ຈຳນວນເຕັມ rlimit_nofile_bump(ຈຳນວນເຕັມ limit);
ຈຳນວນເຕັມ rlimit_nofile_safe(void);

ຈຳນວນເຕັມ pid_getrlimit(pid_t pid, ຈຳນວນເຕັມ resource, sa_nracanaa rlimit *ret);
