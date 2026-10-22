/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"

ਅਚਲ ਅੱਖਰ* strerror_or_eof(ਪੂਰਨ errnum, ਅੱਖਰ *buf, size_t buflen) {
        ਜੇ (errnum != 0)
                ਵਾਪਸ strerror_r(ABS(errnum), buf, buflen);

        ਵਾਪਸ "Unexpected EOF";
}
