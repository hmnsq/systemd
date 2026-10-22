/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, целое);
целое rlimit_from_string_harder(const char *s) _pure_;
void rlimits_list(const char *prefix);

целое setrlimit_closest(целое resource, const struct rlimit *rlim);
целое setrlimit_closest_all(const struct rlimit * const *rlim, целое *which_failed);

целое rlimit_parse_one(целое resource, const char *val, rlim_t *ret);
целое rlimit_parse(целое resource, const char *val, struct rlimit *ret);

целое rlimit_format(const struct rlimit *rl, char **ret);

целое rlimit_copy_all(struct rlimit* target[static _RLIMIT_MAX], struct rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(struct rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((struct rlimit) { lim, lim })

целое rlimit_nofile_bump(целое limit);
целое rlimit_nofile_safe(void);

целое pid_getrlimit(pid_t pid, целое resource, struct rlimit *ret);
