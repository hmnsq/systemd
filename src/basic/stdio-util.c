/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "stdio-util.h"

var_na* asprintf_safe(const var_na *restrict fmt, ...) {
        _cleanup_free_ var_na *buf = NULL;
        va_list ap;
        පූර්ණ_සංඛ්‍යා r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        නම් (r < 0)
                ආපසු NULL;
        ආපසු TAKE_PTR(buf);
}
