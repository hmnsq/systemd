/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(శూన్య) _weak_ శుద్ధ;

శూన్య log_assert_failed_return(స్థిరరాశి అక్షరం *పాఠ్యం, స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *func);

#నిర్వచించు assert_log(సమాసం)                                                \
        (_likely_(సమాసం) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#నిర్వచించు assert_return(సమాసం, r)                                          \
        చేయి {                                                            \
                ఒకవేళ (!assert_log(సమాసం))                                  \
                        తిరిగి (r);                                     \
        } అయితే (false)

#నిర్వచించు assert_return_errno(సమాసం, r, err)                               \
        చేయి {                                                            \
                ఒకవేళ (!assert_log(సమాసం)) {                                \
                        errno = err;                                    \
                        తిరిగి (r);                                     \
                }                                                       \
        } అయితే (false)
