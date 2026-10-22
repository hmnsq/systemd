/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "errno-list.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ errno_name* lookup_errno(ನೋಂದಣಿ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str,
                                             ನೋಂದಣಿ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "errno-from-name.inc"

ಪೂರ್ಣಾಂಕ errno_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ errno_name *sc;

        ಪ್ರತಿಜ್ಞಾ(name);

        sc = lookup_errno(name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
        ಆದರೆ (!sc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಪ್ರತಿಜ್ಞಾ(sc->id > 0);
        ಹಿಂತಿರುಗಿ sc->id;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* errno_name_no_fallback(ಪೂರ್ಣಾಂಕ id) {
        ಆದರೆ (id == 0) /* To stay in line with our implementation below.  */
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ strerrorname_np(ABS(id));
}
#ಇಲ್ಲದಿದ್ದರೆ
#  include "errno-to-name.inc"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* errno_name_no_fallback(ಪೂರ್ಣಾಂಕ id) {
        ಆದರೆ (id < 0)
                id = -id;

        ಆದರೆ ((size_t) id >= ELEMENTSOF(errno_ಹೆಸರುಗಳು))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ errno_ಹೆಸರುಗಳು[id];
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* errno_name(ಪೂರ್ಣಾಂಕ id, ಅಕ್ಷರ buf[ಸ್ಥಿರ ERRNO_NAME_BUF_LEN]) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a = errno_name_no_fallback(id);
        ಆದರೆ (a)
                ಹಿಂತಿರುಗಿ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", ನಿರಪೇಕ್ಷ(id));
        ಹಿಂತಿರುಗಿ buf;
}
