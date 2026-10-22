/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <glob.h>       /* IWYU pragma: export */

#شامل "forward.h"

تعریفنوع DIR* (*opendir_t)(ثابت نویسه *);

صحیح safe_glob_full(ثابت نویسه *path, صحیح flags, opendir_t opendir_func, نویسه ***ret);
ایستا درونخطی صحیح safe_glob(ثابت نویسه *path, صحیح flags, نویسه ***ret) {
        بازگشت safe_glob_full(path, flags, پوچ, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحیح glob_first(ثابت نویسه *path, نویسه **ret);
#define glob_exists(path) glob_first(path, پوچ)
صحیح glob_extend(نویسه ***strv, ثابت نویسه *path, صحیح flags);

صحیح glob_non_glob_prefix(ثابت نویسه *path, نویسه **ret);

bool string_is_glob(ثابت نویسه *p) _pure_;
