/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "stdio-util.h"

वर्ण* asprintf_safe(ध्रुव वर्ण *सीमित fmt, ...) {
        _cleanup_free_ वर्ण *buf = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        यदि (r < 0)
                प्रत्यागम रिक्त;
        प्रत्यागम TAKE_PTR(buf);
}
