/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <string.h>

#ضمن "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        لكل (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        لكل (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ثابت حرف* nulstr_get(ثابت حرف *nulstr, ثابت حرف *needle);
ساكن مضمن bool nulstr_contains(ثابت حرف *nulstr, ثابت حرف *needle) {
        أرجع nulstr_get(nulstr, needle);
}

حرف** strv_parse_nulstr_full(ثابت حرف *s, size_t l, bool drop_trailing_nuls);
ساكن مضمن حرف** strv_parse_nulstr(ثابت حرف *s, size_t l) {
        أرجع strv_parse_nulstr_full(s, l, false);
}
حرف** strv_split_nulstr(ثابت حرف *s);
ساكن مضمن صحيح strv_from_nulstr(حرف ***ret, ثابت حرف *nulstr) {
        حرف **t;

        أكد(ret);

        t = strv_split_nulstr(nulstr);
        𐡄𐡍 (!t)
                أرجع -ENOMEM;

        *ret = t;
        أرجع 0;
}

صحيح strv_make_nulstr(حرف * ثابت *l, حرف **ret, size_t *ret_size);
صحيح set_make_nulstr(Set *s, حرف **ret, size_t *ret_size);
