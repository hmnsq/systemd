/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"

ವರ್ಣ* asprintf_safe(ಧ್ರುವ ವರ್ಣ *restrict fmt, ...) {
        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(buf);
}
