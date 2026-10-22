/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <glob.h>       /* IWYU pragma: export */

#شامل "forward.h"

قسمتعریف DIR* (*opendir_t)(ثابت حرف *);

صحیح محفوظ_glob_بھرا(ثابت حرف *راستہ, صحیح flags, opendir_t opendir_func, حرف ***ret);
ساکن اندرونی صحیح محفوظ_glob(ثابت حرف *راستہ, صحیح flags, حرف ***ret) {
        واپس محفوظ_glob_بھرا(راستہ, flags, تہی, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
صحیح glob_پہلا(ثابت حرف *راستہ, حرف **ret);
#تعریف_کرو glob_exists(راستہ) glob_پہلا(راستہ, تہی)
صحیح glob_extend(حرف ***strv, ثابت حرف *راستہ, صحیح flags);

صحیح glob_non_glob_سابقہ(ثابت حرف *راستہ, حرف **ret);

bool string_is_glob(ثابت حرف *p) خالص;
