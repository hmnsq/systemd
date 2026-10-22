/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, цео);
цео rlimit_from_string_harder(const char *s) _pure_;
void rlimits_list(const char *prefix);

цео setrlimit_closest(цео resource, const struct rlimit *rlim);
цео setrlimit_closest_all(const struct rlimit * const *rlim, цео *which_failed);

цео rlimit_parse_one(цео resource, const char *val, rlim_t *ret);
цео rlimit_parse(цео resource, const char *val, struct rlimit *ret);

цео rlimit_format(const struct rlimit *rl, char **ret);

цео rlimit_copy_all(struct rlimit* target[static _RLIMIT_MAX], struct rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(struct rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((struct rlimit) { lim, lim })

цео rlimit_nofile_bump(цео limit);
цео rlimit_nofile_safe(void);

цео pid_getrlimit(pid_t pid, цео resource, struct rlimit *ret);
