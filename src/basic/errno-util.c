/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "errno-util.h"

ಧ್ರುವ ವರ್ಣ* strerror_or_eof(ಪೂರ್ಣಾಙ್ಕ errnum, ವರ್ಣ *buf, size_t buflen) {
        ಯದಿ (errnum != 0)
                ಪ್ರತ್ಯಾಗಮ strerror_r(ABS(errnum), buf, buflen);

        ಪ್ರತ್ಯಾಗಮ "Unexpected EOF";
}
