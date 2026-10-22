/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "stdio-util.h"

বর্ণ* asprintf_safe(ধ্রুব বর্ণ *restrict fmt, ...) {
        _cleanup_free_ বর্ণ *buf = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        যদি (r < 0)
                প্রত্যাগম রিক্ত;
        প্রত্যাগম TAKE_PTR(buf);
}
