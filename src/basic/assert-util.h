/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(शून्य) _weak_ _pure_;

शून्य log_assert_failed_return(स्थिरांक अक्षर *text, स्थिरांक अक्षर *file, पूर्णांक ओळ, स्थिरांक अक्षर *func);

#define assert_log(अभिव्यक्ती)                                                \
        (_likely_(अभिव्यक्ती) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(अभिव्यक्ती, r)                                          \
        करा {                                                            \
                जर (!assert_log(अभिव्यक्ती))                                  \
                        परत (r);                                     \
        } पर्यंत (false)

#define assert_return_errno(अभिव्यक्ती, r, err)                               \
        करा {                                                            \
                जर (!assert_log(अभिव्यक्ती)) {                                \
                        त्रुटि_सं = err;                                    \
                        परत (r);                                     \
                }                                                       \
        } पर्यंत (false)
