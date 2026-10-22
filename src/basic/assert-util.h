/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(void) _weak_ _pure_;

void log_assert_failed_return(const var_na *text, const var_na *file, ကိန်းပြည့် line, const var_na *func);

#define assert_log(expr)                                                \
        (_likely_(expr) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(expr, r)                                          \
        do {                                                            \
                အကယ်၍ (!assert_log(expr))                                  \
                        ပြန်ပေး (r);                                     \
        } နေစဉ် (false)

#define assert_return_errno(expr, r, err)                               \
        do {                                                            \
                အကယ်၍ (!assert_log(expr)) {                                \
                        errno = err;                                    \
                        ပြန်ပေး (r);                                     \
                }                                                       \
        } နေစဉ် (false)
