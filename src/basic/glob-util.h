/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <glob.h>       /* IWYU pragma: export */

#समावेश "forward.h"

प्रकार सूची* (*opendir_t)(स्थिर अक्षर *);

पूर्णांक safe_glob_full(स्थिर अक्षर *path, पूर्णांक flags, opendir_t opendir_func, अक्षर ***ret);
अटल अंतरभूत पूर्णांक safe_glob(स्थिर अक्षर *path, पूर्णांक flags, अक्षर ***ret) {
        वापस safe_glob_full(path, flags, शून्य, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
पूर्णांक glob_first(स्थिर अक्षर *path, अक्षर **ret);
#घोषणा glob_exists(path) glob_first(path, शून्य)
पूर्णांक glob_extend(अक्षर ***strv, स्थिर अक्षर *path, पूर्णांक flags);

पूर्णांक glob_non_glob_prefix(स्थिर अक्षर *path, अक्षर **ret);

bool string_is_glob(स्थिर अक्षर *p) _pure_;
