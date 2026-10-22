/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "errno-util.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* strerror_or_eof(ಪೂರ್ಣಾಂಕ errnum, ಅಕ್ಷರ *buf, size_t buflen) {
        ಆದರೆ (errnum != 0)
                ಹಿಂತಿರುಗಿ strerror_r(ABS(errnum), buf, buflen);

        ಹಿಂತಿರುಗಿ "Unexpected EOF";
}
