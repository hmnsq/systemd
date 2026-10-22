/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <glob.h>       /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारव्याख्या सूची* (*opendir_t)(स्थिरांक अक्षर *);

पूर्णांक safe_glob_full(स्थिरांक अक्षर *path, पूर्णांक flags, opendir_t opendir_func, अक्षर ***ret);
स्थिर ओळीत पूर्णांक safe_glob(स्थिरांक अक्षर *path, पूर्णांक flags, अक्षर ***ret) {
        परत safe_glob_full(path, flags, रिक्त, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
पूर्णांक glob_first(स्थिरांक अक्षर *path, अक्षर **ret);
#define glob_exists(path) glob_first(path, रिक्त)
पूर्णांक glob_extend(अक्षर ***strv, स्थिरांक अक्षर *path, पूर्णांक flags);

पूर्णांक glob_non_glob_prefix(स्थिरांक अक्षर *path, अक्षर **ret);

bool string_is_glob(स्थिरांक अक्षर *p) _pure_;
