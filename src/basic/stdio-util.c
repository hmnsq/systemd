/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

var_na* asprintf_safe(const var_na *restrict fmt, ...) {
        _cleanup_free_ var_na *buf = NULL;
        va_list ap;
        ចំនួនគត់ r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ប្រសិនបើ (r < 0)
                ត្រឡប់ NULL;
        ត្រឡប់ TAKE_PTR(buf);
}
