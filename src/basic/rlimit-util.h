/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/resource.h>       /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, पूर्णाङ्क);
पूर्णाङ्क rlimit_from_string_harder(अचर वर्ण *s) _pure_;
शून्य rlimits_list(अचर वर्ण *prefix);

पूर्णाङ्क setrlimit_closest(पूर्णाङ्क resource, अचर संरचना rlimit *rlim);
पूर्णाङ्क setrlimit_closest_all(अचर संरचना rlimit * अचर *rlim, पूर्णाङ्क *which_failed);

पूर्णाङ्क rlimit_parse_one(पूर्णाङ्क resource, अचर वर्ण *val, rlim_t *ret);
पूर्णाङ्क rlimit_parse(पूर्णाङ्क resource, अचर वर्ण *val, संरचना rlimit *ret);

पूर्णाङ्क rlimit_format(अचर संरचना rlimit *rl, वर्ण **ret);

पूर्णाङ्क rlimit_copy_all(संरचना rlimit* target[स्थिर _RLIMIT_MAX], संरचना rlimit* अचर source[स्थिर _RLIMIT_MAX]);
शून्य rlimit_free_all(संरचना rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((संरचना rlimit) { lim, lim })

पूर्णाङ्क rlimit_nofile_bump(पूर्णाङ्क limit);
पूर्णाङ्क rlimit_nofile_safe(शून्य);

पूर्णाङ्क pid_getrlimit(pid_t pid, पूर्णाङ्क resource, संरचना rlimit *ret);
