/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "stdio-util.h"

نویسه* asprintf_safe(ثابت نویسه *restrict fmt, ...) {
        _cleanup_free_ نویسه *buf = پوچ;
        va_list ap;
        صحیح r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت پوچ;
        بازگشت TAKE_PTR(buf);
}
