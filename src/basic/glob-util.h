/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <glob.h>       /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع DIR* (*opendir_t)(ثابت حرف *);

صحيح محفوظ_glob_ڀريل(ثابت حرف *رستو, صحيح flags, opendir_t opendir_func, حرف ***ret);
ساكن مضمن صحيح محفوظ_glob(ثابت حرف *رستو, صحيح flags, حرف ***ret) {
        أرجع محفوظ_glob_ڀريل(رستو, flags, عدم, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحيح glob_پهريون(ثابت حرف *رستو, حرف **ret);
#وصف_ڪريو glob_exists(رستو) glob_پهريون(رستو, عدم)
صحيح glob_extend(حرف ***strv, ثابت حرف *رستو, صحيح flags);

صحيح glob_non_glob_اڳياڙي(ثابت حرف *رستو, حرف **ret);

bool string_is_glob(ثابت حرف *p) خالص;
