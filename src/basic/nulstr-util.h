/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <string.h>

#شامل "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        برای (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        برای (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ثابت نویسه* nulstr_get(ثابت نویسه *nulstr, ثابت نویسه *needle);
ایستا درونخطی bool nulstr_contains(ثابت نویسه *nulstr, ثابت نویسه *needle) {
        بازگشت nulstr_get(nulstr, needle);
}

نویسه** strv_parse_nulstr_full(ثابت نویسه *s, size_t l, bool drop_trailing_nuls);
ایستا درونخطی نویسه** strv_parse_nulstr(ثابت نویسه *s, size_t l) {
        بازگشت strv_parse_nulstr_full(s, l, false);
}
نویسه** strv_split_nulstr(ثابت نویسه *s);
ایستا درونخطی صحیح strv_from_nulstr(نویسه ***ret, ثابت نویسه *nulstr) {
        نویسه **t;

        أكد(ret);

        t = strv_split_nulstr(nulstr);
        اگر (!t)
                بازگشت -ENOMEM;

        *ret = t;
        بازگشت 0;
}

صحیح strv_make_nulstr(نویسه * ثابت *l, نویسه **ret, size_t *ret_size);
صحیح set_make_nulstr(Set *s, نویسه **ret, size_t *ret_size);
