/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(व्योम) _weak_ _pure_;

व्योम log_assert_failed_return(स्थिर अक्षर *text, स्थिर अक्षर *file, पूर्णांक पंक्ति, स्थिर अक्षर *func);

#घोषणा assert_log(व्यंजक)                                                \
        (_likely_(व्यंजक) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#घोषणा assert_return(व्यंजक, r)                                          \
        करो {                                                            \
                अगर (!assert_log(व्यंजक))                                  \
                        वापस (r);                                     \
        } जबतक (false)

#घोषणा assert_return_errno(व्यंजक, r, err)                               \
        करो {                                                            \
                अगर (!assert_log(व्यंजक)) {                                \
                        त्रुटि_सं = err;                                    \
                        वापस (r);                                     \
                }                                                       \
        } जबतक (false)
