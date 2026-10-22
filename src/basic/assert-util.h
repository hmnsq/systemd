/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(শূন্য) _weak_ _pure_;

শূন্য log_assert_failed_return(ধ্রুব বর্ণ *text, ধ্রুব বর্ণ *file, পূর্ণাঙ্ক সারি, ধ্রুব বর্ণ *func);

#define assert_log(রাশিমালা)                                                \
        (_likely_(রাশিমালা) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(রাশিমালা, r)                                          \
        কুরু {                                                            \
                যদি (!assert_log(রাশিমালা))                                  \
                        প্রত্যাগম (r);                                     \
        } যাবত্ (false)

#define assert_return_errno(রাশিমালা, r, err)                               \
        কুরু {                                                            \
                যদি (!assert_log(রাশিমালা)) {                                \
                        errno = err;                                    \
                        প্রত্যাগম (r);                                     \
                }                                                       \
        } যাবত্ (false)
