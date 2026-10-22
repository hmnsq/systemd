/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"

ಅಕ್ಷರ* asprintf_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸೀಮಿತ fmt, ...) {
        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ TAKE_PTR(buf);
}
