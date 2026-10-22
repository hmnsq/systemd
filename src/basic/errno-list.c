/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "errno-list.h"

ساكن ثابت بنية errno_name* lookup_errno(سجل ثابت حرف *str,
                                             سجل GPERF_LEN_TYPE len);

#ضمن "errno-from-name.inc"

صحيح errno_from_name(ثابت حرف *name) {
        ثابت بنية errno_name *sc;

        أكد(name);

        sc = lookup_errno(name, strlen(name));
        إذا (!sc)
                إرجاع -EINVAL;

        أكد(sc->id > 0);
        إرجاع sc->id;
}

#ifdef __GLIBC__
ثابت حرف* errno_name_no_fallback(صحيح id) {
        إذا (id == 0) /* To stay in line with our implementation below.  */
                إرجاع عدم;

        إرجاع strerrorname_np(ABS(id));
}
#وإلا
#  include "errno-to-name.inc"

ثابت حرف* errno_name_no_fallback(صحيح id) {
        إذا (id < 0)
                id = -id;

        إذا ((size_t) id >= ELEMENTSOF(errno_names))
                إرجاع عدم;

        إرجاع errno_names[id];
}
#endif

ثابت حرف* errno_name(صحيح id, حرف buf[ساكن ERRNO_NAME_BUF_LEN]) {
        ثابت حرف *a = errno_name_no_fallback(id);
        إذا (a)
                إرجاع a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        إرجاع buf;
}
