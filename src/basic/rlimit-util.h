/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, 整数);
整数 rlimit_from_string_harder(const char *s) _pure_;
void rlimits_list(const char *prefix);

整数 setrlimit_closest(整数 resource, const struct rlimit *rlim);
整数 setrlimit_closest_all(const struct rlimit * const *rlim, 整数 *which_failed);

整数 rlimit_parse_one(整数 resource, const char *val, rlim_t *ret);
整数 rlimit_parse(整数 resource, const char *val, struct rlimit *ret);

整数 rlimit_format(const struct rlimit *rl, char **ret);

整数 rlimit_copy_all(struct rlimit* target[static _RLIMIT_MAX], struct rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(struct rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((struct rlimit) { lim, lim })

整数 rlimit_nofile_bump(整数 limit);
整数 rlimit_nofile_safe(void);

整数 pid_getrlimit(pid_t pid, 整数 resource, struct rlimit *ret);
