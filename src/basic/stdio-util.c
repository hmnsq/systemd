/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "stdio-util.h"

حرف* asprintf_safe(ثابت حرف *restrict fmt, ...) {
        _cleanup_free_ حرف *buf = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ܐܢ (r < 0)
                ܦܢܐ عدم;
        ܦܢܐ TAKE_PTR(buf);
}
