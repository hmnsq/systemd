/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(शून्य) _weak_ _pure_;

शून्य log_assert_failed_return(अचर वर्ण *text, अचर वर्ण *file, पूर्णाङ्क पङ्क्ति, अचर वर्ण *func);

#define assert_log(अभिव्यक्ति)                                                \
        (_likely_(अभिव्यक्ति) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(अभिव्यक्ति, r)                                          \
        गर {                                                            \
                यदि (!assert_log(अभिव्यक्ति))                                  \
                        फर्काऊ (r);                                     \
        } जबसम्म (false)

#define assert_return_errno(अभिव्यक्ति, r, err)                               \
        गर {                                                            \
                यदि (!assert_log(अभिव्यक्ति)) {                                \
                        त्रुटि_सं = err;                                    \
                        फर्काऊ (r);                                     \
                }                                                       \
        } जबसम्म (false)
