/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "stdio-util.h"

ৱর্ণ* asprintf_safe(ধ্রুৱ ৱর্ণ *restrict fmt, ...) {
        _cleanup_free_ ৱর্ণ *buf = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই রিক্ত;
        ঘূৰাই TAKE_PTR(buf);
}
