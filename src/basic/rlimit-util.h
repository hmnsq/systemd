/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <sys/resource.h>       /* IWYU pragma: export */

#ضمن "forward.h"

#تعریف_کړه _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_جدول_LOOKUP(rlimit, صحيح);
صحيح rlimit_from_string_harder(ثابت حرف *s) خالص;
فراغ rlimits_list(ثابت حرف *مختاړی);

صحيح setrlimit_closest(صحيح resource, ثابت بنية rlimit *rlim);
صحيح setrlimit_closest_all(ثابت بنية rlimit * ثابت *rlim, صحيح *which_failed);

صحيح rlimit_parse_one(صحيح resource, ثابت حرف *val, rlim_t *ret);
صحيح rlimit_parse(صحيح resource, ثابت حرف *val, بنية rlimit *ret);

صحيح rlimit_format(ثابت بنية rlimit *rl, حرف **ret);

صحيح rlimit_copy_all(بنية rlimit* موخه[ساكن _RLIMIT_MAX], بنية rlimit* ثابت سرچینه[ساكن _RLIMIT_MAX]);
فراغ rlimit_free_all(بنية rlimit **rl);

#تعریف_کړه RLIMIT_MAKE_CONST(lim) ((بنية rlimit) { lim, lim })

صحيح rlimit_nofile_bump(صحيح limit);
صحيح rlimit_nofile_خوندي(فراغ);

صحيح pid_getrlimit(pid_t pid, صحيح resource, بنية rlimit *ret);
