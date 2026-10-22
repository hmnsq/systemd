/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(فراغ) _weak_ خالص;

فراغ log_assert_failed_return(ثابت حرف *متن, ثابت حرف *file, صحيح سٽ, ثابت حرف *func);

#وصف_ڪريو assert_log(اظهار)                                                \
        (_likely_(اظهار) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#وصف_ڪريو assert_return(اظهار, r)                                          \
        افعل {                                                            \
                إذا (!assert_log(اظهار))                                  \
                        أرجع (r);                                     \
        } طالما (false)

#وصف_ڪريو assert_return_errno(اظهار, r, err)                               \
        افعل {                                                            \
                إذا (!assert_log(اظهار)) {                                \
                        errno = err;                                    \
                        أرجع (r);                                     \
                }                                                       \
        } طالما (false)
