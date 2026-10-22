/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <glob.h>       /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع DIR* (*opendir_t)(ثابت حرف *);

صحيح امن_glob_پر(ثابت حرف *مسیر, صحيح flags, opendir_t opendir_func, حرف ***ret);
ساكن مضمن صحيح امن_glob(ثابت حرف *مسیر, صحيح flags, حرف ***ret) {
        أرجع امن_glob_پر(مسیر, flags, عدم, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحيح glob_اولی(ثابت حرف *مسیر, حرف **ret);
#تعریف_کن glob_exists(مسیر) glob_اولی(مسیر, عدم)
صحيح glob_extend(حرف ***strv, ثابت حرف *مسیر, صحيح flags);

صحيح glob_non_glob_پیشوند(ثابت حرف *مسیر, حرف **ret);

bool string_is_glob(ثابت حرف *p) خالص;
