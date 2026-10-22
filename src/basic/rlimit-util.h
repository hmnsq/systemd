/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/resource.h>       /* IWYU pragma: export */

#समावेश "forward.h"

#घोषणा _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, पूर्णांक);
पूर्णांक rlimit_from_string_harder(स्थिर अक्षर *s) _pure_;
व्योम rlimits_list(स्थिर अक्षर *prefix);

पूर्णांक setrlimit_closest(पूर्णांक resource, स्थिर काष्ठा rlimit *rlim);
पूर्णांक setrlimit_closest_all(स्थिर काष्ठा rlimit * स्थिर *rlim, पूर्णांक *which_failed);

पूर्णांक rlimit_parse_one(पूर्णांक resource, स्थिर अक्षर *val, rlim_t *ret);
पूर्णांक rlimit_parse(पूर्णांक resource, स्थिर अक्षर *val, काष्ठा rlimit *ret);

पूर्णांक rlimit_format(स्थिर काष्ठा rlimit *rl, अक्षर **ret);

पूर्णांक rlimit_copy_all(काष्ठा rlimit* target[अटल _RLIMIT_MAX], काष्ठा rlimit* स्थिर source[अटल _RLIMIT_MAX]);
व्योम rlimit_free_all(काष्ठा rlimit **rl);

#घोषणा RLIMIT_MAKE_CONST(lim) ((काष्ठा rlimit) { lim, lim })

पूर्णांक rlimit_nofile_bump(पूर्णांक limit);
पूर्णांक rlimit_nofile_safe(व्योम);

पूर्णांक pid_getrlimit(pid_t pid, पूर्णांक resource, काष्ठा rlimit *ret);
