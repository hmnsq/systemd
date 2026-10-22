/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

var_na* asprintf_safe(const var_na *restrict fmt, ...) {
        _cleanup_free_ var_na *buf = NULL;
        va_list ap;
        ကိန်းပြည့် r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး NULL;
        ပြန်ပေး TAKE_PTR(buf);
}
