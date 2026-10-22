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
        אם (!sc)
                החזר -EINVAL;

        أكد(sc->id > 0);
        החזר sc->id;
}

#ifdef __GLIBC__
ثابت حرف* errno_name_no_fallback(صحيح id) {
        אם (id == 0) /* To stay in line with our implementation below.  */
                החזר عدم;

        החזר strerrorname_np(ABS(id));
}
#אחרת
#  include "errno-to-name.inc"

ثابت حرف* errno_name_no_fallback(صحيح id) {
        אם (id < 0)
                id = -id;

        אם ((size_t) id >= ELEMENTSOF(errno_names))
                החזר عدم;

        החזר errno_names[id];
}
#endif

ثابت حرف* errno_name(صحيح id, حرف buf[ساكن ERRNO_NAME_BUF_LEN]) {
        ثابت حرف *a = errno_name_no_fallback(id);
        אם (a)
                החזר a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        החזר buf;
}
