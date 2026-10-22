/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "errno-list.h"

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ errno_name* lookup_errno(ಪಞ್ಜಿಕಾ ಧ್ರುವ ವರ್ಣ *str,
                                             ಪಞ್ಜಿಕಾ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "errno-from-name.inc"

ಪೂರ್ಣಾಙ್ಕ errno_from_name(ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ಸಂರಚನಾ errno_name *sc;

        ಪ್ರತಿಜ್ಞಾ(name);

        sc = lookup_errno(name, strlen(name));
        ಯದಿ (!sc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತಿಜ್ಞಾ(sc->id > 0);
        ಪ್ರತ್ಯಾಗಮ sc->id;
}

#ifdef __GLIBC__
ಧ್ರುವ ವರ್ಣ* errno_name_no_fallback(ಪೂರ್ಣಾಙ್ಕ id) {
        ಯದಿ (id == 0) /* To stay in line with our implementation below.  */
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ strerrorname_np(ABS(id));
}
#ಅನ್ಯಥಾ
#  include "errno-to-name.inc"

ಧ್ರುವ ವರ್ಣ* errno_name_no_fallback(ಪೂರ್ಣಾಙ್ಕ id) {
        ಯದಿ (id < 0)
                id = -id;

        ಯದಿ ((size_t) id >= ELEMENTSOF(errno_names))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ errno_names[id];
}
#endif

ಧ್ರುವ ವರ್ಣ* errno_name(ಪೂರ್ಣಾಙ್ಕ id, ವರ್ಣ buf[ಸ್ಥಿರ ERRNO_NAME_BUF_LEN]) {
        ಧ್ರುವ ವರ್ಣ *a = errno_name_no_fallback(id);
        ಯದಿ (a)
                ಪ್ರತ್ಯಾಗಮ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ಪ್ರತ್ಯಾಗಮ buf;
}
