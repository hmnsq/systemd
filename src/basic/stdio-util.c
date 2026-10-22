/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "stdio-util.h"

वर्ण* asprintf_safe(अचर वर्ण *restrict fmt, ...) {
        _cleanup_free_ वर्ण *buf = रिक्त;
        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        बहु_पूर्ण(ap);

        यदि (r < 0)
                फर्काऊ रिक्त;
        फर्काऊ TAKE_PTR(buf);
}
