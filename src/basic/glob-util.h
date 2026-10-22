/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <glob.h>       /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع DIR* (*opendir_t)(ثابت حرف *);

صحيح safe_glob_full(ثابت حرف *path, صحيح flags, opendir_t opendir_func, حرف ***ret);
ساكن مضمن صحيح safe_glob(ثابت حرف *path, صحيح flags, حرف ***ret) {
        ܦܢܐ safe_glob_full(path, flags, عدم, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحيح glob_first(ثابت حرف *path, حرف **ret);
#define glob_exists(path) glob_first(path, عدم)
صحيح glob_extend(حرف ***strv, ثابت حرف *path, صحيح flags);

صحيح glob_non_glob_prefix(ثابت حرف *path, حرف **ret);

bool string_is_glob(ثابت حرف *p) _pure_;
