/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <string.h>

#ضمن "forward.h"

#تعریف_کړه NULSTR_FOREACH(i, l)                                    \
        لكل (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#تعریف_کړه NULSTR_FOREACH_PAIR(i, j, l)                             \
        لكل (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ثابت حرف* nulstr_get(ثابت حرف *nulstr, ثابت حرف *needle);
ساكن مضمن bool nulstr_لري(ثابت حرف *nulstr, ثابت حرف *needle) {
        أرجع nulstr_get(nulstr, needle);
}

حرف** strv_parse_nulstr_ډک(ثابت حرف *s, size_t l, bool drop_trailing_nuls);
ساكن مضمن حرف** strv_parse_nulstr(ثابت حرف *s, size_t l) {
        أرجع strv_parse_nulstr_ډک(s, l, false);
}
حرف** strv_وېشل_nulstr(ثابت حرف *s);
ساكن مضمن صحيح strv_from_nulstr(حرف ***ret, ثابت حرف *nulstr) {
        حرف **t;

        أكد(ret);

        t = strv_وېشل_nulstr(nulstr);
        إذا (!t)
                أرجع -ENOMEM;

        *ret = t;
        أرجع 0;
}

صحيح strv_make_nulstr(حرف * ثابت *l, حرف **ret, size_t *ret_اندازه);
صحيح سېټ_make_nulstr(سېټ_2 *s, حرف **ret, size_t *ret_اندازه);
