/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "stdio-util.h"

अक्षर* asprintf_safe(स्थिर अक्षर *restrict fmt, ...) {
        _cleanup_free_ अक्षर *buf = शून्य;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस शून्य;
        वापस TAKE_PTR(buf);
}
