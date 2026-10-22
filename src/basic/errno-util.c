/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "errno-util.h"

ثابت نویسه* strerror_or_eof(صحیح errnum, نویسه *buf, size_t buflen) {
        اگر (errnum != 0)
                بازگشت strerror_r(ABS(errnum), buf, buflen);

        بازگشت "Unexpected EOF";
}
