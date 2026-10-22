/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, จำนวนเต็ม);
จำนวนเต็ม rlimit_from_string_harder(const var_na *s) _pure_;
void rlimits_list(const var_na *prefix);

จำนวนเต็ม setrlimit_closest(จำนวนเต็ม resource, const sa_nracanaa rlimit *rlim);
จำนวนเต็ม setrlimit_closest_all(const sa_nracanaa rlimit * const *rlim, จำนวนเต็ม *which_failed);

จำนวนเต็ม rlimit_parse_one(จำนวนเต็ม resource, const var_na *val, rlim_t *ret);
จำนวนเต็ม rlimit_parse(จำนวนเต็ม resource, const var_na *val, sa_nracanaa rlimit *ret);

จำนวนเต็ม rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret);

จำนวนเต็ม rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(sa_nracanaa rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((sa_nracanaa rlimit) { lim, lim })

จำนวนเต็ม rlimit_nofile_bump(จำนวนเต็ม limit);
จำนวนเต็ม rlimit_nofile_safe(void);

จำนวนเต็ม pid_getrlimit(pid_t pid, จำนวนเต็ม resource, sa_nracanaa rlimit *ret);
