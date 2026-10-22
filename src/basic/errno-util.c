/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "errno-util.h"

ثابت حرف* strerror_or_eof(صحيح errnum, حرف *buf, size_t buflen) {
        𐡄𐡍 (errnum != 0)
                أرجع strerror_r(ABS(errnum), buf, buflen);

        أرجع "Unexpected EOF";
}
