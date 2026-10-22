/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(فراغ) _weak_ _pure_;

فراغ log_assert_failed_return(ثابت حرف *text, ثابت حرف *file, صحيح line, ثابت حرف *func);

#define assert_log(expr)                                                \
        (_likely_(expr) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(expr, r)                                          \
        𐤐𐤏𐤋 {                                                            \
                𐤀𐤌 (!assert_log(expr))                                  \
                        أرجع (r);                                     \
        } طالما (false)

#define assert_return_errno(expr, r, err)                               \
        𐤐𐤏𐤋 {                                                            \
                𐤀𐤌 (!assert_log(expr)) {                                \
                        errno = err;                                    \
                        أرجع (r);                                     \
                }                                                       \
        } طالما (false)
