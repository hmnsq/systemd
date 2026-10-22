/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(ಶೂನ್ಯ) _weak_ ಶುದ್ಧ;

ಶೂನ್ಯ log_assert_failed_return(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func);

#ವ್ಯಾಖ್ಯಾನಿಸು assert_log(ಅಭಿವ್ಯಕ್ತಿ)                                                \
        (_likely_(ಅಭಿವ್ಯಕ್ತಿ) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#ವ್ಯಾಖ್ಯಾನಿಸು assert_return(ಅಭಿವ್ಯಕ್ತಿ, r)                                          \
        ಮಾಡು {                                                            \
                ಆದರೆ (!assert_log(ಅಭಿವ್ಯಕ್ತಿ))                                  \
                        ಹಿಂತಿರುಗಿ (r);                                     \
        } ಆಗ (false)

#ವ್ಯಾಖ್ಯಾನಿಸು assert_return_errno(ಅಭಿವ್ಯಕ್ತಿ, r, err)                               \
        ಮಾಡು {                                                            \
                ಆದರೆ (!assert_log(ಅಭಿವ್ಯಕ್ತಿ)) {                                \
                        errno = err;                                    \
                        ಹಿಂತಿರುಗಿ (r);                                     \
                }                                                       \
        } ಆಗ (false)
