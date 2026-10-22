/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <string.h>

#شامل "forward.h"

#تعریف_کرو NULSTR_FOREACH(i, l)                                    \
        برائے (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#تعریف_کرو NULSTR_FOREACH_PAIR(i, j, l)                             \
        برائے (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ثابت حرف* nulstr_get(ثابت حرف *nulstr, ثابت حرف *needle);
ساکن اندرونی bool nulstr_شامل_ہے(ثابت حرف *nulstr, ثابت حرف *needle) {
        واپس nulstr_get(nulstr, needle);
}

حرف** strv_parse_nulstr_بھرا(ثابت حرف *s, size_t l, bool drop_trailing_nuls);
ساکن اندرونی حرف** strv_parse_nulstr(ثابت حرف *s, size_t l) {
        واپس strv_parse_nulstr_بھرا(s, l, false);
}
حرف** strv_بانٹو_nulstr(ثابت حرف *s);
ساکن اندرونی صحیح strv_from_nulstr(حرف ***ret, ثابت حرف *nulstr) {
        حرف **t;

        أكد(ret);

        t = strv_بانٹو_nulstr(nulstr);
        اگر (!t)
                واپس -ENOMEM;

        *ret = t;
        واپس 0;
}

صحیح strv_make_nulstr(حرف * ثابت *l, حرف **ret, size_t *ret_سائز);
صحیح سیٹ_make_nulstr(سیٹ_2 *s, حرف **ret, size_t *ret_سائز);
