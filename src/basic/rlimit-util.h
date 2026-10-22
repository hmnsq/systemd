/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/resource.h>       /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(rlimit, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ rlimit_from_string_harder(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) ಶುದ್ಧ;
ಶೂನ್ಯ rlimits_list(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ);

ಪೂರ್ಣಾಂಕ setrlimit_closest(ಪೂರ್ಣಾಂಕ resource, ಸ್ಥಿರಾಂಕ ರಚನೆ rlimit *rlim);
ಪೂರ್ಣಾಂಕ setrlimit_closest_all(ಸ್ಥಿರಾಂಕ ರಚನೆ rlimit * ಸ್ಥಿರಾಂಕ *rlim, ಪೂರ್ಣಾಂಕ *which_failed);

ಪೂರ್ಣಾಂಕ rlimit_parse_one(ಪೂರ್ಣಾಂಕ resource, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret);
ಪೂರ್ಣಾಂಕ rlimit_parse(ಪೂರ್ಣಾಂಕ resource, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, ರಚನೆ rlimit *ret);

ಪೂರ್ಣಾಂಕ rlimit_format(ಸ್ಥಿರಾಂಕ ರಚನೆ rlimit *rl, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ rlimit_copy_all(ರಚನೆ rlimit* ಗುರಿ[ಸ್ಥಿರ _RLIMIT_MAX], ರಚನೆ rlimit* ಸ್ಥಿರಾಂಕ ಮೂಲ[ಸ್ಥಿರ _RLIMIT_MAX]);
ಶೂನ್ಯ rlimit_free_all(ರಚನೆ rlimit **rl);

#ವ್ಯಾಖ್ಯಾನಿಸು RLIMIT_MAKE_CONST(lim) ((ರಚನೆ rlimit) { lim, lim })

ಪೂರ್ಣಾಂಕ rlimit_nofile_bump(ಪೂರ್ಣಾಂಕ limit);
ಪೂರ್ಣಾಂಕ rlimit_nofile_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ pid_getrlimit(pid_t pid, ಪೂರ್ಣಾಂಕ resource, ರಚನೆ rlimit *ret);
