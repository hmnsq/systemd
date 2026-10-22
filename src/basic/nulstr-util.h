/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        জন্য (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        জন্য (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ধ্রুবক অক্ষর* nulstr_get(ধ্রুবক অক্ষর *nulstr, ধ্রুবক অক্ষর *needle);
স্থির অন্তঃরেখ bool nulstr_contains(ধ্রুবক অক্ষর *nulstr, ধ্রুবক অক্ষর *needle) {
        ফেরত nulstr_get(nulstr, needle);
}

অক্ষর** strv_parse_nulstr_full(ধ্রুবক অক্ষর *s, size_t l, bool drop_trailing_nuls);
স্থির অন্তঃরেখ অক্ষর** strv_parse_nulstr(ধ্রুবক অক্ষর *s, size_t l) {
        ফেরত strv_parse_nulstr_full(s, l, false);
}
অক্ষর** strv_split_nulstr(ধ্রুবক অক্ষর *s);
স্থির অন্তঃরেখ পূর্ণসংখ্যা strv_from_nulstr(অক্ষর ***ret, ধ্রুবক অক্ষর *nulstr) {
        অক্ষর **t;

        প্রতিজ্ঞা(ret);

        t = strv_split_nulstr(nulstr);
        যদি (!t)
                ফেরত -ENOMEM;

        *ret = t;
        ফেরত 0;
}

পূর্ণসংখ্যা strv_make_nulstr(অক্ষর * ধ্রুবক *l, অক্ষর **ret, size_t *ret_আকার);
পূর্ণসংখ্যা set_make_nulstr(Set *s, অক্ষর **ret, size_t *ret_আকার);
