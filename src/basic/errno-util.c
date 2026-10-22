/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "errno-util.h"

ধ্রুৱ ৱর্ণ* strerror_or_eof(পূর্ণাঙ্ক errnum, ৱর্ণ *buf, size_t buflen) {
        যদি (errnum != 0)
                ঘূৰাই strerror_r(ABS(errnum), buf, buflen);

        ঘূৰাই "Unexpected EOF";
}
