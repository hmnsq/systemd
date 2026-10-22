/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/resource.h>       /* IWYU pragma: export */

#include "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, số_nguyên);
số_nguyên rlimit_from_string_harder(const char *s) _pure_;
void rlimits_list(const char *prefix);

số_nguyên setrlimit_closest(số_nguyên resource, const struct rlimit *rlim);
số_nguyên setrlimit_closest_all(const struct rlimit * const *rlim, số_nguyên *which_failed);

số_nguyên rlimit_parse_one(số_nguyên resource, const char *val, rlim_t *ret);
số_nguyên rlimit_parse(số_nguyên resource, const char *val, struct rlimit *ret);

số_nguyên rlimit_format(const struct rlimit *rl, char **ret);

số_nguyên rlimit_copy_all(struct rlimit* target[static _RLIMIT_MAX], struct rlimit* const source[static _RLIMIT_MAX]);
void rlimit_free_all(struct rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((struct rlimit) { lim, lim })

số_nguyên rlimit_nofile_bump(số_nguyên limit);
số_nguyên rlimit_nofile_safe(void);

số_nguyên pid_getrlimit(pid_t pid, số_nguyên resource, struct rlimit *ret);
