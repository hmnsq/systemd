/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, ціле);
ціле rlimit_from_string_harder(const char *s) _pure_;
void rlimits_list(const char *prefix);

ціле setrlimit_closest(ціле resource, const struct rlimit *rlim);
ціле setrlimit_closest_all(const struct rlimit * const *rlim, ціле *which_failed);

ціле rlimit_parse_one(ціле resource, const char *val, rlim_t *ret);
ціле rlimit_parse(ціле resource, const char *val, struct rlimit *ret);

ціле rlimit_format(const struct rlimit *rl, char **ret);

ціле rlimit_copy_all(struct rlimit* target[static _RLIMIT_MAX], struct rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(struct rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((struct rlimit) { lim, lim })

ціле rlimit_nofile_bump(ціле limit);
ціле rlimit_nofile_safe(void);

ціле pid_getrlimit(pid_t pid, ціле resource, struct rlimit *ret);
