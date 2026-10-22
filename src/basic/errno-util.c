/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(ཧྲིལ errnum, var_na *buf, size_t buflen) {
        གལ་ཏེ (errnum != 0)
                ཕྱིར strerror_r(ABS(errnum), buf, buflen);

        ཕྱིར "Unexpected EOF";
}
