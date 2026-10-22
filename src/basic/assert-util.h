/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(void) _weak_ _pure_;

void log_assert_failed_return(const char *text, const char *file, int लिन्या, const char *func);

#define assert_log(पाहायाग)                                                \
        (_likely_(पाहायाग) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(पाहायाग, r)                                          \
        do {                                                            \
                if (!assert_log(पाहायाग))                                  \
                        return (r);                                     \
        } while (false)

#define assert_return_errno(पाहायाग, r, err)                               \
        do {                                                            \
                if (!assert_log(पाहायाग)) {                                \
                        errno = err;                                    \
                        return (r);                                     \
                }                                                       \
        } while (false)
