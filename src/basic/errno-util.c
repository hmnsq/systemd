/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(정수 errnum, char *buf, size_t buflen) {
        만약 (errnum != 0)
                반환 strerror_r(ABS(errnum), buf, buflen);

        반환 "Unexpected EOF";
}
