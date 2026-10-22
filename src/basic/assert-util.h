/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(શૂન્ય) _weak_ _pure_;

શૂન્ય log_assert_failed_return(અચલ અક્ષર *text, અચલ અક્ષર *file, પૂર્ણાંક લીટી, અચલ અક્ષર *func);

#define assert_log(પદાવલિ)                                                \
        (_likely_(પદાવલિ) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(પદાવલિ, r)                                          \
        કરો {                                                            \
                જો (!assert_log(પદાવલિ))                                  \
                        પાછા (r);                                     \
        } જ્યારે (false)

#define assert_return_errno(પદાવલિ, r, err)                               \
        કરો {                                                            \
                જો (!assert_log(પદાવલિ)) {                                \
                        errno = err;                                    \
                        પાછા (r);                                     \
                }                                                       \
        } જ્યારે (false)
