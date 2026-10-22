/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        প্রতি (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        প্রতি (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ধ্রুব বর্ণ* nulstr_get(ধ্রুব বর্ণ *nulstr, ধ্রুব বর্ণ *needle);
স্থির অন্তঃস্থ bool nulstr_contains(ধ্রুব বর্ণ *nulstr, ধ্রুব বর্ণ *needle) {
        প্রত্যাগম nulstr_get(nulstr, needle);
}

বর্ণ** strv_parse_nulstr_full(ধ্রুব বর্ণ *s, size_t l, bool drop_trailing_nuls);
স্থির অন্তঃস্থ বর্ণ** strv_parse_nulstr(ধ্রুব বর্ণ *s, size_t l) {
        প্রত্যাগম strv_parse_nulstr_full(s, l, false);
}
বর্ণ** strv_split_nulstr(ধ্রুব বর্ণ *s);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_from_nulstr(বর্ণ ***ret, ধ্রুব বর্ণ *nulstr) {
        বর্ণ **t;

        প্রতিজ্ঞা(ret);

        t = strv_split_nulstr(nulstr);
        যদি (!t)
                প্রত্যাগম -ENOMEM;

        *ret = t;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক strv_make_nulstr(বর্ণ * ধ্রুব *l, বর্ণ **ret, size_t *ret_size);
পূর্ণাঙ্ক set_make_nulstr(Set *s, বর্ণ **ret, size_t *ret_size);
