/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>

#شامل "errno-list.h"

ایستا ثابت ساختار errno_name* lookup_errno(ثبات ثابت نویسه *str,
                                             ثبات GPERF_LEN_TYPE len);

#شامل "errno-from-name.inc"

صحیح errno_from_name(ثابت نویسه *name) {
        ثابت ساختار errno_name *sc;

        أكد(name);

        sc = lookup_errno(name, strlen(name));
        اگر (!sc)
                بازگشت -EINVAL;

        أكد(sc->id > 0);
        بازگشت sc->id;
}

#ifdef __GLIBC__
ثابت نویسه* errno_name_no_fallback(صحیح id) {
        اگر (id == 0) /* To stay in line with our implementation below.  */
                بازگشت پوچ;

        بازگشت strerrorname_np(ABS(id));
}
#وگرنه
#  include "errno-to-name.inc"

ثابت نویسه* errno_name_no_fallback(صحیح id) {
        اگر (id < 0)
                id = -id;

        اگر ((size_t) id >= ELEMENTSOF(errno_names))
                بازگشت پوچ;

        بازگشت errno_names[id];
}
#endif

ثابت نویسه* errno_name(صحیح id, نویسه buf[ایستا ERRNO_NAME_BUF_LEN]) {
        ثابت نویسه *a = errno_name_no_fallback(id);
        اگر (a)
                بازگشت a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        بازگشت buf;
}
