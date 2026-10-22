/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(शून्य) _weak_ _pure_;

शून्य log_assert_failed_return(ध्रुव वर्ण *text, ध्रुव वर्ण *file, पूर्णाङ्क ओळ, ध्रुव वर्ण *func);

#define assert_log(अभिव्यक्ती)                                                \
        (_likely_(अभिव्यक्ती) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(अभिव्यक्ती, r)                                          \
        कुरु {                                                            \
                यदि (!assert_log(अभिव्यक्ती))                                  \
                        प्रत्यागम (r);                                     \
        } यावत् (false)

#define assert_return_errno(अभिव्यक्ती, r, err)                               \
        कुरु {                                                            \
                यदि (!assert_log(अभिव्यक्ती)) {                                \
                        errno = err;                                    \
                        प्रत्यागम (r);                                     \
                }                                                       \
        } यावत् (false)
