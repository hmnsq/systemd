/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        க்கு (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        க்கு (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

மாறிலி எழுத்து* nulstr_get(மாறிலி எழுத்து *nulstr, மாறிலி எழுத்து *needle);
நிலையான உள்வரி bool nulstr_contains(மாறிலி எழுத்து *nulstr, மாறிலி எழுத்து *needle) {
        திரும்பு nulstr_get(nulstr, needle);
}

எழுத்து** strv_parse_nulstr_full(மாறிலி எழுத்து *s, size_t l, bool drop_trailing_nuls);
நிலையான உள்வரி எழுத்து** strv_parse_nulstr(மாறிலி எழுத்து *s, size_t l) {
        திரும்பு strv_parse_nulstr_full(s, l, false);
}
எழுத்து** strv_split_nulstr(மாறிலி எழுத்து *s);
நிலையான உள்வரி முழு strv_from_nulstr(எழுத்து ***ret, மாறிலி எழுத்து *nulstr) {
        எழுத்து **t;

        ப்ரதிஜ்ஞா(ret);

        t = strv_split_nulstr(nulstr);
        எனில் (!t)
                திரும்பு -ENOMEM;

        *ret = t;
        திரும்பு 0;
}

முழு strv_make_nulstr(எழுத்து * மாறிலி *l, எழுத்து **ret, size_t *ret_size);
முழு set_make_nulstr(Set *s, எழுத்து **ret, size_t *ret_size);
