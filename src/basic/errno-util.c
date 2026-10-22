/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(ကိန်းပြည့် errnum, var_na *buf, size_t buflen) {
        အကယ်၍ (errnum != 0)
                ပြန်ပေး strerror_r(ABS(errnum), buf, buflen);

        ပြန်ပေး "Unexpected EOF";
}
