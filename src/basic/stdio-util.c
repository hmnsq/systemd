/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

char* asprintf_safe(const char *restrict fmt, ...) {
        _cleanup_free_ char *buf = NULL;
        va_list ap;
        số_nguyên r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về NULL;
        trả_về TAKE_PTR(buf);
}
