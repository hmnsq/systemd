/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "errno-util.h"

ধ্রুব বর্ণ* strerror_or_eof(পূর্ণাঙ্ক errnum, বর্ণ *buf, size_t buflen) {
        যদি (errnum != 0)
                প্রত্যাগম strerror_r(ABS(errnum), buf, buflen);

        প্রত্যাগম "Unexpected EOF";
}
