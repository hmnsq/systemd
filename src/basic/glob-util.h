/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <glob.h>       /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع DIR* (*opendir_t)(ثابت حرف *);

صحيح خوندي_glob_ډک(ثابت حرف *لار, صحيح flags, opendir_t opendir_func, حرف ***ret);
ساكن مضمن صحيح خوندي_glob(ثابت حرف *لار, صحيح flags, حرف ***ret) {
        أرجع خوندي_glob_ډک(لار, flags, عدم, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحيح glob_لومړی(ثابت حرف *لار, حرف **ret);
#تعریف_کړه glob_exists(لار) glob_لومړی(لار, عدم)
صحيح glob_extend(حرف ***strv, ثابت حرف *لار, صحيح flags);

صحيح glob_non_glob_مختاړی(ثابت حرف *لار, حرف **ret);

bool string_is_glob(ثابت حرف *p) خالص;
