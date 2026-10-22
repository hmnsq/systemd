/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>

#شامل "errno-list.h"

ساکن ثابت ساخت errno_name* lookup_errno(اندراج ثابت حرف *str,
                                             اندراج GPERF_LEN_TYPE len);

#شامل "errno-from-name.inc"

صحیح errno_from_name(ثابت حرف *name) {
        ثابت ساخت errno_name *sc;

        أكد(name);

        sc = lookup_errno(name, لڑی_لمبائی(name));
        اگر (!sc)
                واپس -EINVAL;

        أكد(sc->id > 0);
        واپس sc->id;
}

#اگر_تعریف_شدہ __GLIBC__
ثابت حرف* errno_name_no_fallback(صحیح id) {
        اگر (id == 0) /* To stay in line with our implementation below.  */
                واپس تہی;

        واپس strerrorname_np(ABS(id));
}
#ورنہ
#  include "errno-to-name.inc"

ثابت حرف* errno_name_no_fallback(صحیح id) {
        اگر (id < 0)
                id = -id;

        اگر ((size_t) id >= ELEMENTSOF(errno_نام))
                واپس تہی;

        واپس errno_نام[id];
}
#اگر_ختم

ثابت حرف* errno_name(صحیح id, حرف buf[ساکن ERRNO_NAME_BUF_LEN]) {
        ثابت حرف *a = errno_name_no_fallback(id);
        اگر (a)
                واپس a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", مطلق_قدر(id));
        واپس buf;
}
