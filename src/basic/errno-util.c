/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(ចំនួនគត់ errnum, var_na *buf, size_t buflen) {
        ប្រសិនបើ (errnum != 0)
                ត្រឡប់ strerror_r(ABS(errnum), buf, buflen);

        ត្រឡប់ "Unexpected EOF";
}
