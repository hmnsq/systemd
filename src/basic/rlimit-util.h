/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/resource.h>       /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_from_string_harder(ଅଚଳ ଅକ୍ଷର *s) _pure_;
ଶୂନ୍ୟ rlimits_list(ଅଚଳ ଅକ୍ଷର *prefix);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setrlimit_closest(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ଅଚଳ ସଂରଚନା rlimit *rlim);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା setrlimit_closest_all(ଅଚଳ ସଂରଚନା rlimit * ଅଚଳ *rlim, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *which_failed);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ଅଚଳ ଅକ୍ଷର *val, ସଂରଚନା rlimit *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_format(ଅଚଳ ସଂରଚନା rlimit *rl, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_copy_all(ସଂରଚନା rlimit* target[ସ୍ଥିର _RLIMIT_MAX], ସଂରଚନା rlimit* ଅଚଳ source[ସ୍ଥିର _RLIMIT_MAX]);
ଶୂନ୍ୟ rlimit_free_all(ସଂରଚନା rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((ସଂରଚନା rlimit) { lim, lim })

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_nofile_bump(ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_nofile_safe(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_getrlimit(pid_t pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ସଂରଚନା rlimit *ret);
