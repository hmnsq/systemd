/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(خالی) _weak_ خالص;

خالی log_assert_failed_return(ثابت حرف *متن, ثابت حرف *file, صحیح سطر, ثابت حرف *func);

#تعریف_کرو assert_log(اظہاریہ)                                                \
        (_likely_(اظہاریہ) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#تعریف_کرو assert_return(اظہاریہ, r)                                          \
        کرو {                                                            \
                اگر (!assert_log(اظہاریہ))                                  \
                        واپس (r);                                     \
        } جب (false)

#تعریف_کرو assert_return_errno(اظہاریہ, r, err)                               \
        کرو {                                                            \
                اگر (!assert_log(اظہاریہ)) {                                \
                        errno = err;                                    \
                        واپس (r);                                     \
                }                                                       \
        } جب (false)
