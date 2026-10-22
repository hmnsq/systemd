/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "stdio-util.h"

حرف* asprintf_محفوظ(ثابت حرف *محدود fmt, ...) {
        _cleanup_free_ حرف *buf = تہی;
        va_list ap;
        صحیح r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        اگر (r < 0)
                واپس تہی;
        واپس TAKE_PTR(buf);
}
