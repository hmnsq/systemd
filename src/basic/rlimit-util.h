/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/resource.h>       /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, পূর্ণসংখ্যা);
পূর্ণসংখ্যা rlimit_from_string_harder(ধ্রুবক অক্ষর *s) _pure_;
শূন্য rlimits_list(ধ্রুবক অক্ষর *prefix);

পূর্ণসংখ্যা setrlimit_closest(পূর্ণসংখ্যা resource, ধ্রুবক কাঠামো rlimit *rlim);
পূর্ণসংখ্যা setrlimit_closest_all(ধ্রুবক কাঠামো rlimit * ধ্রুবক *rlim, পূর্ণসংখ্যা *which_failed);

পূর্ণসংখ্যা rlimit_parse_one(পূর্ণসংখ্যা resource, ধ্রুবক অক্ষর *val, rlim_t *ret);
পূর্ণসংখ্যা rlimit_parse(পূর্ণসংখ্যা resource, ধ্রুবক অক্ষর *val, কাঠামো rlimit *ret);

পূর্ণসংখ্যা rlimit_format(ধ্রুবক কাঠামো rlimit *rl, অক্ষর **ret);

পূর্ণসংখ্যা rlimit_copy_all(কাঠামো rlimit* target[স্থির _RLIMIT_MAX], কাঠামো rlimit* ধ্রুবক source[স্থির _RLIMIT_MAX]);
শূন্য rlimit_free_all(কাঠামো rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((কাঠামো rlimit) { lim, lim })

পূর্ণসংখ্যা rlimit_nofile_bump(পূর্ণসংখ্যা limit);
পূর্ণসংখ্যা rlimit_nofile_safe(শূন্য);

পূর্ণসংখ্যা pid_getrlimit(pid_t pid, পূর্ণসংখ্যা resource, কাঠামো rlimit *ret);
