/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        ಪ್ರತಿ (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        ಪ್ರತಿ (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ಧ್ರುವ ವರ್ಣ* nulstr_get(ಧ್ರುವ ವರ್ಣ *nulstr, ಧ್ರುವ ವರ್ಣ *needle);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool nulstr_contains(ಧ್ರುವ ವರ್ಣ *nulstr, ಧ್ರುವ ವರ್ಣ *needle) {
        ಪ್ರತ್ಯಾಗಮ nulstr_get(nulstr, needle);
}

ವರ್ಣ** strv_parse_nulstr_full(ಧ್ರುವ ವರ್ಣ *s, size_t l, bool drop_trailing_nuls);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ** strv_parse_nulstr(ಧ್ರುವ ವರ್ಣ *s, size_t l) {
        ಪ್ರತ್ಯಾಗಮ strv_parse_nulstr_full(s, l, false);
}
ವರ್ಣ** strv_split_nulstr(ಧ್ರುವ ವರ್ಣ *s);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strv_from_nulstr(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *nulstr) {
        ವರ್ಣ **t;

        ಪ್ರತಿಜ್ಞಾ(ret);

        t = strv_split_nulstr(nulstr);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = t;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ strv_make_nulstr(ವರ್ಣ * ಧ್ರುವ *l, ವರ್ಣ **ret, size_t *ret_size);
ಪೂರ್ಣಾಙ್ಕ set_make_nulstr(Set *s, ವರ್ಣ **ret, size_t *ret_size);
