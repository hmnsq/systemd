/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

var_na* asprintf_safe(const var_na *restrict fmt, ...) {
        _cleanup_free_ var_na *buf = NULL;
        va_list ap;
        ຈຳນວນເຕັມ r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ຖ້າ (r < 0)
                ສົ່ງຄືນ NULL;
        ສົ່ງຄືນ TAKE_PTR(buf);
}
