/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(تهی) _weak_ _pure_;

تهی log_assert_failed_return(ثابت نویسه *text, ثابت نویسه *file, صحیح سطر, ثابت نویسه *func);

#define assert_log(عبارت)                                                \
        (_likely_(عبارت) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(عبارت, r)                                          \
        انجامبده {                                                            \
                اگر (!assert_log(عبارت))                                  \
                        بازگشت (r);                                     \
        } وقتی (false)

#define assert_return_errno(عبارت, r, err)                               \
        انجامبده {                                                            \
                اگر (!assert_log(عبارت)) {                                \
                        errno = err;                                    \
                        بازگشت (r);                                     \
                }                                                       \
        } وقتی (false)
