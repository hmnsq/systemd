/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"

ଅକ୍ଷର* asprintf_safe(ଅଚଳ ଅକ୍ଷର *restrict fmt, ...) {
        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ ଖାଲି;
        ଫେରନ୍ତୁ TAKE_PTR(buf);
}
