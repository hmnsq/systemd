/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(শূন্য) _weak_ _pure_;

শূন্য log_assert_failed_return(ধ্রুবক অক্ষর *text, ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *func);

#define assert_log(রাশিমালা)                                                \
        (_likely_(রাশিমালা) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(রাশিমালা, r)                                          \
        করো {                                                            \
                যদি (!assert_log(রাশিমালা))                                  \
                        ফেরত (r);                                     \
        } যখন (false)

#define assert_return_errno(রাশিমালা, r, err)                               \
        করো {                                                            \
                যদি (!assert_log(রাশিমালা)) {                                \
                        errno = err;                                    \
                        ফেরত (r);                                     \
                }                                                       \
        } যখন (false)
