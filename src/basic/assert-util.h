/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(শূন্য) _weak_ _pure_;

শূন্য log_assert_failed_return(ধ্রুৱ ৱর্ণ *text, ধ্রুৱ ৱর্ণ *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *func);

#define assert_log(ৰাশিমালা)                                                \
        (_likely_(ৰাশিমালা) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(ৰাশিমালা, r)                                          \
        কৰা {                                                            \
                যদি (!assert_log(ৰাশিমালা))                                  \
                        ঘূৰাই (r);                                     \
        } যেতিয়ালৈ (false)

#define assert_return_errno(ৰাশিমালা, r, err)                               \
        কৰা {                                                            \
                যদি (!assert_log(ৰাশিমালা)) {                                \
                        errno = err;                                    \
                        ঘূৰাই (r);                                     \
                }                                                       \
        } যেতিয়ালৈ (false)
