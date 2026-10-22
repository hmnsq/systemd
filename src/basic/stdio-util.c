/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "stdio-util.h"

वर्ण* asprintf_safe(ध्रुव वर्ण *restrict fmt, ...) {
        _cleanup_free_ वर्ण *buf = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        सचे (r < 0)
                पटिनिवत्त रिक्त;
        पटिनिवत्त TAKE_PTR(buf);
}
