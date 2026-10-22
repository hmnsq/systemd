/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

ثابت حرف* errno_name_no_fallback(صحيح id) _const_;
صحيح errno_from_name(ثابت حرف *name) خالص;

ساكن مضمن bool errno_is_valid(صحيح n) {
        أرجع n > 0 && n <= ERRNO_MAX;
}

#تعریف_کن ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(صحيح)
/* Like errno_name, but always returns a string. */
ثابت حرف* errno_name(صحيح id, حرف buf[ساكن ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#تعریف_کن ERRNO_NAME(errnum) errno_name(errnum, (حرف[ERRNO_NAME_BUF_LEN]){})
