/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(ຈຳນວນເຕັມ errnum, var_na *buf, size_t buflen) {
        ຖ້າ (errnum != 0)
                ສົ່ງຄືນ strerror_r(ABS(errnum), buf, buflen);

        ສົ່ງຄືນ "Unexpected EOF";
}
