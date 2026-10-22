/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

ধ্রুবক অক্ষর* errno_name_no_fallback(পূর্ণসংখ্যা id) _const_;
পূর্ণসংখ্যা errno_from_name(ধ্রুবক অক্ষর *name) _pure_;

স্থির অন্তঃরেখ bool errno_is_valid(পূর্ণসংখ্যা n) {
        ফেরত n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(পূর্ণসংখ্যা)
/* Like errno_name, but always returns a string. */
ধ্রুবক অক্ষর* errno_name(পূর্ণসংখ্যা id, অক্ষর buf[স্থির ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (অক্ষর[ERRNO_NAME_BUF_LEN]){})
