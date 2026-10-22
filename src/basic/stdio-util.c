/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "stdio-util.h"

অক্ষর* asprintf_safe(ধ্রুবক অক্ষর *restrict fmt, ...) {
        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        যদি (r < 0)
                ফেরত ফাঁকা;
        ফেরত TAKE_PTR(buf);
}
