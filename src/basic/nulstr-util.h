/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        বাবে (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        বাবে (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ধ্রুৱ ৱর্ণ* nulstr_get(ধ্রুৱ ৱর্ণ *nulstr, ধ্রুৱ ৱর্ণ *needle);
স্থির অন্তঃস্থ bool nulstr_contains(ধ্রুৱ ৱর্ণ *nulstr, ধ্রুৱ ৱর্ণ *needle) {
        ঘূৰাই nulstr_get(nulstr, needle);
}

ৱর্ণ** strv_parse_nulstr_full(ধ্রুৱ ৱর্ণ *s, size_t l, bool drop_trailing_nuls);
স্থির অন্তঃস্থ ৱর্ণ** strv_parse_nulstr(ধ্রুৱ ৱর্ণ *s, size_t l) {
        ঘূৰাই strv_parse_nulstr_full(s, l, false);
}
ৱর্ণ** strv_split_nulstr(ধ্রুৱ ৱর্ণ *s);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_from_nulstr(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *nulstr) {
        ৱর্ণ **t;

        প্রতিজ্ঞা(ret);

        t = strv_split_nulstr(nulstr);
        যদি (!t)
                ঘূৰাই -ENOMEM;

        *ret = t;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক strv_make_nulstr(ৱর্ণ * ধ্রুৱ *l, ৱর্ণ **ret, size_t *ret_size);
পূর্ণাঙ্ক set_make_nulstr(Set *s, ৱর্ণ **ret, size_t *ret_size);
