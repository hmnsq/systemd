/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "stdio-util.h"

અક્ષર* asprintf_safe(અચલ અક્ષર *restrict fmt, ...) {
        _cleanup_free_ અક્ષર *buf = ખાલી;
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        જો (r < 0)
                પાછા ખાલી;
        પાછા TAKE_PTR(buf);
}
