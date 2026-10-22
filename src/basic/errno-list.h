/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

ثابت نویسه* errno_name_no_fallback(صحیح id) _const_;
صحیح errno_from_name(ثابت نویسه *name) _pure_;

ایستا درونخطی bool errno_is_valid(صحیح n) {
        بازگشت n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(صحیح)
/* Like errno_name, but always returns a string. */
ثابت نویسه* errno_name(صحیح id, نویسه buf[ایستا ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (نویسه[ERRNO_NAME_BUF_LEN]){})
