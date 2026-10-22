/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/resource.h>       /* IWYU pragma: export */

#شامل "forward.h"

#تعریف_کرو _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_جدول_LOOKUP(rlimit, صحیح);
صحیح rlimit_from_string_harder(ثابت حرف *s) خالص;
خالی rlimits_list(ثابت حرف *سابقہ);

صحیح setrlimit_closest(صحیح resource, ثابت ساخت rlimit *rlim);
صحیح setrlimit_closest_all(ثابت ساخت rlimit * ثابت *rlim, صحیح *which_failed);

صحیح rlimit_parse_one(صحیح resource, ثابت حرف *val, rlim_t *ret);
صحیح rlimit_parse(صحیح resource, ثابت حرف *val, ساخت rlimit *ret);

صحیح rlimit_format(ثابت ساخت rlimit *rl, حرف **ret);

صحیح rlimit_copy_all(ساخت rlimit* ہدف[ساکن _RLIMIT_MAX], ساخت rlimit* ثابت ماخذ[ساکن _RLIMIT_MAX]);
خالی rlimit_free_all(ساخت rlimit **rl);

#تعریف_کرو RLIMIT_MAKE_CONST(lim) ((ساخت rlimit) { lim, lim })

صحیح rlimit_nofile_bump(صحیح limit);
صحیح rlimit_nofile_محفوظ(خالی);

صحیح pid_getrlimit(pid_t pid, صحیح resource, ساخت rlimit *ret);
