/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const char* strerror_or_eof(цео errnum, char *buf, size_t buflen) {
        ако (errnum != 0)
                врати strerror_r(ABS(errnum), buf, buflen);

        врати "Unexpected EOF";
}
