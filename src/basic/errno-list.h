/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

ثابت حرف* errno_name_no_fallback(صحیح id) _const_;
صحیح errno_from_name(ثابت حرف *name) خالص;

ساکن اندرونی bool errno_is_valid(صحیح n) {
        واپس n > 0 && n <= ERRNO_MAX;
}

#تعریف_کرو ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(صحیح)
/* Like errno_name, but always returns a string. */
ثابت حرف* errno_name(صحیح id, حرف buf[ساکن ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#تعریف_کرو ERRNO_NAME(errnum) errno_name(errnum, (حرف[ERRNO_NAME_BUF_LEN]){})
