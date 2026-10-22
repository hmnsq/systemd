/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/resource.h>       /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ rlimit_from_string_harder(ಧ್ರುವ ವರ್ಣ *s) _pure_;
ಶೂನ್ಯ rlimits_list(ಧ್ರುವ ವರ್ಣ *prefix);

ಪೂರ್ಣಾಙ್ಕ setrlimit_closest(ಪೂರ್ಣಾಙ್ಕ resource, ಧ್ರುವ ಸಂರಚನಾ rlimit *rlim);
ಪೂರ್ಣಾಙ್ಕ setrlimit_closest_all(ಧ್ರುವ ಸಂರಚನಾ rlimit * ಧ್ರುವ *rlim, ಪೂರ್ಣಾಙ್ಕ *which_failed);

ಪೂರ್ಣಾಙ್ಕ rlimit_parse_one(ಪೂರ್ಣಾಙ್ಕ resource, ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret);
ಪೂರ್ಣಾಙ್ಕ rlimit_parse(ಪೂರ್ಣಾಙ್ಕ resource, ಧ್ರುವ ವರ್ಣ *val, ಸಂರಚನಾ rlimit *ret);

ಪೂರ್ಣಾಙ್ಕ rlimit_format(ಧ್ರುವ ಸಂರಚನಾ rlimit *rl, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ rlimit_copy_all(ಸಂರಚನಾ rlimit* target[ಸ್ಥಿರ _RLIMIT_MAX], ಸಂರಚನಾ rlimit* ಧ್ರುವ source[ಸ್ಥಿರ _RLIMIT_MAX]);
ಶೂನ್ಯ rlimit_free_all(ಸಂರಚನಾ rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((ಸಂರಚನಾ rlimit) { lim, lim })

ಪೂರ್ಣಾಙ್ಕ rlimit_nofile_bump(ಪೂರ್ಣಾಙ್ಕ limit);
ಪೂರ್ಣಾಙ್ಕ rlimit_nofile_safe(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ pid_getrlimit(pid_t pid, ಪೂರ್ಣಾಙ್ಕ resource, ಸಂರಚನಾ rlimit *ret);
