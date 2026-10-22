/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        ପାଇଁ (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        ପାଇଁ (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ଅଚଳ ଅକ୍ଷର* nulstr_get(ଅଚଳ ଅକ୍ଷର *nulstr, ଅଚଳ ଅକ୍ଷର *needle);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool nulstr_contains(ଅଚଳ ଅକ୍ଷର *nulstr, ଅଚଳ ଅକ୍ଷର *needle) {
        ଫେରନ୍ତୁ nulstr_get(nulstr, needle);
}

ଅକ୍ଷର** strv_parse_nulstr_full(ଅଚଳ ଅକ୍ଷର *s, size_t l, bool drop_trailing_nuls);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର** strv_parse_nulstr(ଅଚଳ ଅକ୍ଷର *s, size_t l) {
        ଫେରନ୍ତୁ strv_parse_nulstr_full(s, l, false);
}
ଅକ୍ଷର** strv_split_nulstr(ଅଚଳ ଅକ୍ଷର *s);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_from_nulstr(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *nulstr) {
        ଅକ୍ଷର **t;

        ପ୍ରତିଜ୍ଞା(ret);

        t = strv_split_nulstr(nulstr);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = t;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_make_nulstr(ଅକ୍ଷର * ଅଚଳ *l, ଅକ୍ଷର **ret, size_t *ret_size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_make_nulstr(Set *s, ଅକ୍ଷର **ret, size_t *ret_size);
