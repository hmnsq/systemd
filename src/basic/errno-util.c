/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "errno-util.h"

ثابت حرف* strerror_or_eof(صحيح errnum, حرف *buf, size_t buflen) {
        ܐܢ (errnum != 0)
                ܦܢܐ strerror_r(ABS(errnum), buf, buflen);

        ܦܢܐ "Unexpected EOF";
}
