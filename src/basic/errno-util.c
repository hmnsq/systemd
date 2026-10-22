/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(整数 errnum, char *buf, size_t buflen) {
        もし (errnum != 0)
                戻る strerror_r(ABS(errnum), buf, buflen);

        戻る "Unexpected EOF";
}
