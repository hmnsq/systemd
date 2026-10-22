/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "errno-util.h"

மாறிலி எழுத்து* strerror_or_eof(முழு errnum, எழுத்து *buf, size_t buflen) {
        எனில் (errnum != 0)
                திரும்பு strerror_r(ABS(errnum), buf, buflen);

        திரும்பு "Unexpected EOF";
}
