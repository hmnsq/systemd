/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "errno-util.h"

અચલ અક્ષર* strerror_or_eof(પૂર્ણાંક errnum, અક્ષર *buf, size_t buflen) {
        જો (errnum != 0)
                પાછા strerror_r(ABS(errnum), buf, buflen);

        પાછા "Unexpected EOF";
}
