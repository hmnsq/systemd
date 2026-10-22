/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "errno-util.h"

const var_na* strerror_or_eof(puur_naa_nka errnum, var_na *buf, size_t buflen) {
        ᱡᱩᱫᱤ (errnum != 0)
                ᱨᱩᱣᱟᱹᱲ strerror_r(ABS(errnum), buf, buflen);

        ᱨᱩᱣᱟᱹᱲ "Unexpected EOF";
}
