/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/resource.h>       /* IWYU pragma: export */

#شامل "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, صحیح);
صحیح rlimit_from_string_harder(ثابت نویسه *s) _pure_;
تهی rlimits_list(ثابت نویسه *prefix);

صحیح setrlimit_closest(صحیح resource, ثابت ساختار rlimit *rlim);
صحیح setrlimit_closest_all(ثابت ساختار rlimit * ثابت *rlim, صحیح *which_failed);

صحیح rlimit_parse_one(صحیح resource, ثابت نویسه *val, rlim_t *ret);
صحیح rlimit_parse(صحیح resource, ثابت نویسه *val, ساختار rlimit *ret);

صحیح rlimit_format(ثابت ساختار rlimit *rl, نویسه **ret);

صحیح rlimit_copy_all(ساختار rlimit* target[ایستا _RLIMIT_MAX], ساختار rlimit* ثابت source[ایستا _RLIMIT_MAX]);
تهی rlimit_free_all(ساختار rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((ساختار rlimit) { lim, lim })

صحیح rlimit_nofile_bump(صحیح limit);
صحیح rlimit_nofile_safe(تهی);

صحیح pid_getrlimit(pid_t pid, صحیح resource, ساختار rlimit *ret);
