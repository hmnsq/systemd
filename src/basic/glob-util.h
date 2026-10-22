/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <glob.h>       /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع DIR* (*opendir_t)(ثابت حرف *);

صحيح محفوظ_glob_بھریا(ثابت حرف *رستہ, صحيح flags, opendir_t opendir_func, حرف ***ret);
ساكن مضمن صحيح محفوظ_glob(ثابت حرف *رستہ, صحيح flags, حرف ***ret) {
        أرجع محفوظ_glob_بھریا(رستہ, flags, عدم, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحيح glob_پہلا(ثابت حرف *رستہ, حرف **ret);
#تعریف_کرو glob_exists(رستہ) glob_پہلا(رستہ, عدم)
صحيح glob_extend(حرف ***strv, ثابت حرف *رستہ, صحيح flags);

صحيح glob_non_glob_اگیتر(ثابت حرف *رستہ, حرف **ret);

bool string_is_glob(ثابت حرف *p) خالص;
