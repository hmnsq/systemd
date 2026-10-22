/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <string.h>

#अन्तर्भाव "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        साठी (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = म_अक्षर((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        साठी (typeof(*(l)) *(i) = (l), *(j) = म_अक्षर((i), 0)+1; (i) && *(i); (i) = म_अक्षर((j), 0)+1, (j) = *(i) ? म_अक्षर((i), 0)+1 : (i))

स्थिरांक अक्षर* nulstr_get(स्थिरांक अक्षर *nulstr, स्थिरांक अक्षर *needle);
स्थिर ओळीत bool nulstr_contains(स्थिरांक अक्षर *nulstr, स्थिरांक अक्षर *needle) {
        परत nulstr_get(nulstr, needle);
}

अक्षर** strv_parse_nulstr_full(स्थिरांक अक्षर *s, माप_प्रकार l, bool drop_trailing_nuls);
स्थिर ओळीत अक्षर** strv_parse_nulstr(स्थिरांक अक्षर *s, माप_प्रकार l) {
        परत strv_parse_nulstr_full(s, l, false);
}
अक्षर** strv_split_nulstr(स्थिरांक अक्षर *s);
स्थिर ओळीत पूर्णांक strv_from_nulstr(अक्षर ***ret, स्थिरांक अक्षर *nulstr) {
        अक्षर **t;

        प्रतिज्ञा(ret);

        t = strv_split_nulstr(nulstr);
        जर (!t)
                परत -ENOMEM;

        *ret = t;
        परत 0;
}

पूर्णांक strv_make_nulstr(अक्षर * स्थिरांक *l, अक्षर **ret, माप_प्रकार *ret_size);
पूर्णांक set_make_nulstr(Set *s, अक्षर **ret, माप_प्रकार *ret_size);
