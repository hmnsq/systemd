/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(số_nguyên errnum, char *buf, size_t buflen) {
        nếu (errnum != 0)
                trả_về strerror_r(ABS(errnum), buf, buflen);

        trả_về "Unexpected EOF";
}
