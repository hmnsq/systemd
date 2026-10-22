/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(शून्य) _weak_ _pure_;

शून्य log_assert_failed_return(ध्रुव वर्ण *text, ध्रुव वर्ण *file, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *func);

#define assert_log(व्यंजक)                                                \
        (_likely_(व्यंजक) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(व्यंजक, r)                                          \
        कुरु {                                                            \
                यदि (!assert_log(व्यंजक))                                  \
                        प्रत्यागम (r);                                     \
        } यावत् (false)

#define assert_return_errno(व्यंजक, r, err)                               \
        कुरु {                                                            \
                यदि (!assert_log(व्यंजक)) {                                \
                        errno = err;                                    \
                        प्रत्यागम (r);                                     \
                }                                                       \
        } यावत् (false)
