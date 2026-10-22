/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "errno-util.h"

ଅଚଳ ଅକ୍ଷର* strerror_or_eof(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errnum, ଅକ୍ଷର *buf, size_t buflen) {
        ଯଦି (errnum != 0)
                ଫେରନ୍ତୁ strerror_r(ABS(errnum), buf, buflen);

        ଫେରନ୍ତୁ "Unexpected EOF";
}
