/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(จำนวนเต็ม errnum, var_na *buf, size_t buflen) {
        ถ้า (errnum != 0)
                คืน strerror_r(ABS(errnum), buf, buflen);

        คืน "Unexpected EOF";
}
