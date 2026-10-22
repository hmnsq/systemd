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

        sc = lookup_errno(name, لڑی_لمبائی(name));
        إذا (!sc)
                أرجع -EINVAL;

        أكد(sc->id > 0);
        أرجع sc->id;
}

#جے_تعریف_شدہ __GLIBC__
ثابت حرف* errno_name_no_fallback(صحيح id) {
        إذا (id == 0) /* To stay in line with our implementation below.  */
                أرجع عدم;

        أرجع strerrorname_np(ABS(id));
}
#وإلا
#  include "errno-to-name.inc"

ثابت حرف* errno_name_no_fallback(صحيح id) {
        إذا (id < 0)
                id = -id;

        إذا ((size_t) id >= ELEMENTSOF(errno_ناں))
                أرجع عدم;

        أرجع errno_ناں[id];
}
#جے_انت

ثابت حرف* errno_name(صحيح id, حرف buf[ساكن ERRNO_NAME_BUF_LEN]) {
        ثابت حرف *a = errno_name_no_fallback(id);
        إذا (a)
                أرجع a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", مطلق_قدر(id));
        أرجع buf;
}
