/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "errno-util.h"

ثابت حرف* strerror_or_eof(صحيح errnum, حرف *buf, size_t buflen) {
        אם (errnum != 0)
                החזר strerror_r(ABS(errnum), buf, buflen);

        החזר "Unexpected EOF";
}
