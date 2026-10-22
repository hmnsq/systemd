/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(ಶೂನ್ಯ) _weak_ _pure_;

ಶೂನ್ಯ log_assert_failed_return(ಧ್ರುವ ವರ್ಣ *text, ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *func);

#define assert_log(ಅಭಿವ್ಯಕ್ತಿ)                                                \
        (_likely_(ಅಭಿವ್ಯಕ್ತಿ) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(ಅಭಿವ್ಯಕ್ತಿ, r)                                          \
        ಕುರು {                                                            \
                ಯದಿ (!assert_log(ಅಭಿವ್ಯಕ್ತಿ))                                  \
                        ಪ್ರತ್ಯಾಗಮ (r);                                     \
        } ಯಾವತ್ (false)

#define assert_return_errno(ಅಭಿವ್ಯಕ್ತಿ, r, err)                               \
        ಕುರು {                                                            \
                ಯದಿ (!assert_log(ಅಭಿವ್ಯಕ್ತಿ)) {                                \
                        errno = err;                                    \
                        ಪ್ರತ್ಯಾಗಮ (r);                                     \
                }                                                       \
        } ಯಾವತ್ (false)
