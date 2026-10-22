/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(puur_naa_nka errnum, var_na *buf, size_t buflen) {
        ᯀᯮᯝ (errnum != 0)
                ᯀᯮᯞ strerror_r(ABS(errnum), buf, buflen);

        ᯀᯮᯞ "Unexpected EOF";
}
