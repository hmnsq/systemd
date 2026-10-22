/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

ধ্রুব বর্ণ* errno_name_no_fallback(পূর্ণাঙ্ক id) _const_;
পূর্ণাঙ্ক errno_from_name(ধ্রুব বর্ণ *name) _pure_;

স্থির অন্তঃস্থ bool errno_is_valid(পূর্ণাঙ্ক n) {
        প্রত্যাগম n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(পূর্ণাঙ্ক)
/* Like errno_name, but always returns a string. */
ধ্রুব বর্ণ* errno_name(পূর্ণাঙ্ক id, বর্ণ buf[স্থির ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (বর্ণ[ERRNO_NAME_BUF_LEN]){})
