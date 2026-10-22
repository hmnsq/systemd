/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"

ਅੱਖਰ* asprintf_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਸੀਮਿਤ fmt, ...) {
        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ ਨਿਰਾ;
        ਵਾਪਸ TAKE_PTR(buf);
}
