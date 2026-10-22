/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

char* asprintf_safe(const char *restrict fmt, ...) {
        _cleanup_free_ char *buf = NULL;
        va_list ap;
        정수 r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        만약 (r < 0)
                반환 NULL;
        반환 TAKE_PTR(buf);
}
