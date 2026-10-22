/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "stdio-util.h"

எழுத்து* asprintf_safe(மாறிலி எழுத்து *restrict fmt, ...) {
        _cleanup_free_ எழுத்து *buf = வெற்று;
        va_list ap;
        முழு r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு வெற்று;
        திரும்பு TAKE_PTR(buf);
}
