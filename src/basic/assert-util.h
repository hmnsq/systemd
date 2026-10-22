/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(فراغ) _weak_ خالص;

فراغ log_assert_failed_return(ثابت حرف *متن, ثابت حرف *file, صحيح کرښه, ثابت حرف *func);

#تعریف_کړه assert_log(بیان)                                                \
        (_likely_(بیان) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#تعریف_کړه assert_return(بیان, r)                                          \
        افعل {                                                            \
                إذا (!assert_log(بیان))                                  \
                        أرجع (r);                                     \
        } طالما (false)

#تعریف_کړه assert_return_errno(بیان, r, err)                               \
        افعل {                                                            \
                إذا (!assert_log(بیان)) {                                \
                        errno = err;                                    \
                        أرجع (r);                                     \
                }                                                       \
        } طالما (false)
