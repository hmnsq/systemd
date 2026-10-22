/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/resource.h>       /* IWYU pragma: export */

#ضمن "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, صحيح);
صحيح rlimit_from_string_harder(ثابت حرف *s) _pure_;
فراغ rlimits_list(ثابت حرف *prefix);

صحيح setrlimit_closest(صحيح resource, ثابت بنية rlimit *rlim);
صحيح setrlimit_closest_all(ثابت بنية rlimit * ثابت *rlim, صحيح *which_failed);

صحيح rlimit_parse_one(صحيح resource, ثابت حرف *val, rlim_t *ret);
صحيح rlimit_parse(صحيح resource, ثابت حرف *val, بنية rlimit *ret);

صحيح rlimit_format(ثابت بنية rlimit *rl, حرف **ret);

صحيح rlimit_copy_all(بنية rlimit* target[ساكن _RLIMIT_MAX], بنية rlimit* ثابت source[ساكن _RLIMIT_MAX]);
فراغ rlimit_free_all(بنية rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((بنية rlimit) { lim, lim })

صحيح rlimit_nofile_bump(صحيح limit);
صحيح rlimit_nofile_safe(فراغ);

صحيح pid_getrlimit(pid_t pid, صحيح resource, بنية rlimit *ret);
