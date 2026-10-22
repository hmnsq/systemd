/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(فراغ) _weak_ خالص;

فراغ log_assert_failed_return(ثابت حرف *متن, ثابت حرف *file, صحيح سطر, ثابت حرف *func);

#تعریف_کن assert_log(عبارت)                                                \
        (_likely_(عبارت) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#تعریف_کن assert_return(عبارت, r)                                          \
        افعل {                                                            \
                إذا (!assert_log(عبارت))                                  \
                        أرجع (r);                                     \
        } طالما (false)

#تعریف_کن assert_return_errno(عبارت, r, err)                               \
        افعل {                                                            \
                إذا (!assert_log(عبارت)) {                                \
                        errno = err;                                    \
                        أرجع (r);                                     \
                }                                                       \
        } طالما (false)
