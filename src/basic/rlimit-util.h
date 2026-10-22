/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <sys/resource.h>       /* IWYU pragma: export */

#অন্তর্ভাৱ "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, পূর্ণাঙ্ক);
পূর্ণাঙ্ক rlimit_from_string_harder(ধ্রুৱ ৱর্ণ *s) _pure_;
শূন্য rlimits_list(ধ্রুৱ ৱর্ণ *prefix);

পূর্ণাঙ্ক setrlimit_closest(পূর্ণাঙ্ক resource, ধ্রুৱ সংরচনা rlimit *rlim);
পূর্ণাঙ্ক setrlimit_closest_all(ধ্রুৱ সংরচনা rlimit * ধ্রুৱ *rlim, পূর্ণাঙ্ক *which_failed);

পূর্ণাঙ্ক rlimit_parse_one(পূর্ণাঙ্ক resource, ধ্রুৱ ৱর্ণ *val, rlim_t *ret);
পূর্ণাঙ্ক rlimit_parse(পূর্ণাঙ্ক resource, ধ্রুৱ ৱর্ণ *val, সংরচনা rlimit *ret);

পূর্ণাঙ্ক rlimit_format(ধ্রুৱ সংরচনা rlimit *rl, ৱর্ণ **ret);

পূর্ণাঙ্ক rlimit_copy_all(সংরচনা rlimit* target[স্থির _RLIMIT_MAX], সংরচনা rlimit* ধ্রুৱ source[স্থির _RLIMIT_MAX]);
শূন্য rlimit_free_all(সংরচনা rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((সংরচনা rlimit) { lim, lim })

পূর্ণাঙ্ক rlimit_nofile_bump(পূর্ণাঙ্ক limit);
পূর্ণাঙ্ক rlimit_nofile_safe(শূন্য);

পূর্ণাঙ্ক pid_getrlimit(pid_t pid, পূর্ণাঙ্ক resource, সংরচনা rlimit *ret);
