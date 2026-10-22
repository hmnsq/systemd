/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(ଶୂନ୍ୟ) _weak_ _pure_;

ଶୂନ୍ୟ log_assert_failed_return(ଅଚଳ ଅକ୍ଷର *text, ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *func);

#define assert_log(ପଦସମଷ୍ଟି)                                                \
        (_likely_(ପଦସମଷ୍ଟି) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(ପଦସମଷ୍ଟି, r)                                          \
        କର {                                                            \
                ଯଦି (!assert_log(ପଦସମଷ୍ଟି))                                  \
                        ଫେରନ୍ତୁ (r);                                     \
        } ଯେତେବେଳେ (false)

#define assert_return_errno(ପଦସମଷ୍ଟି, r, err)                               \
        କର {                                                            \
                ଯଦି (!assert_log(ପଦସମଷ୍ଟି)) {                                \
                        errno = err;                                    \
                        ଫେରନ୍ତୁ (r);                                     \
                }                                                       \
        } ଯେତେବେଳେ (false)
