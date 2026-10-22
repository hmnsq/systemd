/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "stdio-util.h"

अक्षर* asprintf_safe(स्थिरांक अक्षर *restrict fmt, ...) {
        _cleanup_free_ अक्षर *buf = रिक्त;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत रिक्त;
        परत TAKE_PTR(buf);
}
