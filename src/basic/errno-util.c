/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "errno-util.h"

ধ্রুবক অক্ষর* strerror_or_eof(পূর্ণসংখ্যা errnum, অক্ষর *buf, size_t buflen) {
        যদি (errnum != 0)
                ফেরত strerror_r(ABS(errnum), buf, buflen);

        ফেরত "Unexpected EOF";
}
