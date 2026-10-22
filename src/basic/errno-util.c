/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "errno-util.h"

ثابت حرف* strerror_or_eof(صحیح errnum, حرف *buf, size_t buflen) {
        اگر (errnum != 0)
                واپس strerror_r(ABS(errnum), buf, buflen);

        واپس "Unexpected EOF";
}
