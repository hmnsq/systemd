/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(整数 errnum, char *buf, size_t buflen) {
        如果 (errnum != 0)
                返回 strerror_r(ABS(errnum), buf, buflen);

        返回 "Unexpected EOF";
}
