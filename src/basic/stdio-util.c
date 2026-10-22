/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

char* asprintf_safe(const char *restrict fmt, ...) {
        _cleanup_free_ char *buf = NULL;
        va_list ap;
        整数 r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        もし (r < 0)
                戻る NULL;
        戻る TAKE_PTR(buf);
}
