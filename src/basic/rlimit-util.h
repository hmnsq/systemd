/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/resource.h>       /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, पूर्णांक);
पूर्णांक rlimit_from_string_harder(स्थिरांक अक्षर *s) _pure_;
शून्य rlimits_list(स्थिरांक अक्षर *prefix);

पूर्णांक setrlimit_closest(पूर्णांक resource, स्थिरांक संरचना rlimit *rlim);
पूर्णांक setrlimit_closest_all(स्थिरांक संरचना rlimit * स्थिरांक *rlim, पूर्णांक *which_failed);

पूर्णांक rlimit_parse_one(पूर्णांक resource, स्थिरांक अक्षर *val, rlim_t *ret);
पूर्णांक rlimit_parse(पूर्णांक resource, स्थिरांक अक्षर *val, संरचना rlimit *ret);

पूर्णांक rlimit_format(स्थिरांक संरचना rlimit *rl, अक्षर **ret);

पूर्णांक rlimit_copy_all(संरचना rlimit* target[स्थिर _RLIMIT_MAX], संरचना rlimit* स्थिरांक source[स्थिर _RLIMIT_MAX]);
शून्य rlimit_free_all(संरचना rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((संरचना rlimit) { lim, lim })

पूर्णांक rlimit_nofile_bump(पूर्णांक limit);
पूर्णांक rlimit_nofile_safe(शून्य);

पूर्णांक pid_getrlimit(pid_t pid, पूर्णांक resource, संरचना rlimit *ret);
