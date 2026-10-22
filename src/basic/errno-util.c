/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(целое errnum, char *buf, size_t buflen) {
        если (errnum != 0)
                вернуть strerror_r(ABS(errnum), buf, buflen);

        вернуть "Unexpected EOF";
}
