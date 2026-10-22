/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(فراغ) _weak_ _pure_;

فراغ log_assert_failed_return(ثابت حرف *text, ثابت حرف *file, صحيح هێڵ, ثابت حرف *func);

#define assert_log(دەربڕین)                                                \
        (_likely_(دەربڕین) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(دەربڕین, r)                                          \
        افعل {                                                            \
                إذا (!assert_log(دەربڕین))                                  \
                        أرجع (r);                                     \
        } طالما (false)

#define assert_return_errno(دەربڕین, r, err)                               \
        افعل {                                                            \
                إذا (!assert_log(دەربڕین)) {                                \
                        errno = err;                                    \
                        أرجع (r);                                     \
                }                                                       \
        } طالما (false)
