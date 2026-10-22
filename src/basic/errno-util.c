/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(ціле errnum, char *buf, size_t buflen) {
        якщо (errnum != 0)
                повернути strerror_r(ABS(errnum), buf, buflen);

        повернути "Unexpected EOF";
}
