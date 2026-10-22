/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

ধ্রুৱ ৱর্ণ* errno_name_no_fallback(পূর্ণাঙ্ক id) _const_;
পূর্ণাঙ্ক errno_from_name(ধ্রুৱ ৱর্ণ *name) _pure_;

স্থির অন্তঃস্থ bool errno_is_valid(পূর্ণাঙ্ক n) {
        ঘূৰাই n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(পূর্ণাঙ্ক)
/* Like errno_name, but always returns a string. */
ধ্রুৱ ৱর্ণ* errno_name(পূর্ণাঙ্ক id, ৱর্ণ buf[স্থির ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (ৱর্ণ[ERRNO_NAME_BUF_LEN]){})
