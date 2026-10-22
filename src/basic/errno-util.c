/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(පූර්ණ_සංඛ්‍යා errnum, var_na *buf, size_t buflen) {
        නම් (errnum != 0)
                ආපසු strerror_r(ABS(errnum), buf, buflen);

        ආපසු "Unexpected EOF";
}
