/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <string.h>

#समावेश "forward.h"

#घोषणा NULSTR_FOREACH(i, l)                                    \
        क्रम (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = म_अक्षर((i), 0)+1)

#घोषणा NULSTR_FOREACH_PAIR(i, j, l)                             \
        क्रम (typeof(*(l)) *(i) = (l), *(j) = म_अक्षर((i), 0)+1; (i) && *(i); (i) = म_अक्षर((j), 0)+1, (j) = *(i) ? म_अक्षर((i), 0)+1 : (i))

स्थिर अक्षर* nulstr_get(स्थिर अक्षर *nulstr, स्थिर अक्षर *needle);
अटल अंतरभूत bool nulstr_contains(स्थिर अक्षर *nulstr, स्थिर अक्षर *needle) {
        वापस nulstr_get(nulstr, needle);
}

अक्षर** strv_parse_nulstr_full(स्थिर अक्षर *s, माप_प्रकार l, bool drop_trailing_nuls);
अटल अंतरभूत अक्षर** strv_parse_nulstr(स्थिर अक्षर *s, माप_प्रकार l) {
        वापस strv_parse_nulstr_full(s, l, false);
}
अक्षर** strv_split_nulstr(स्थिर अक्षर *s);
अटल अंतरभूत पूर्णांक strv_from_nulstr(अक्षर ***ret, स्थिर अक्षर *nulstr) {
        अक्षर **t;

        निश्चित(ret);

        t = strv_split_nulstr(nulstr);
        अगर (!t)
                वापस -ENOMEM;

        *ret = t;
        वापस 0;
}

पूर्णांक strv_make_nulstr(अक्षर * स्थिर *l, अक्षर **ret, माप_प्रकार *ret_आकार);
पूर्णांक set_make_nulstr(Set *s, अक्षर **ret, माप_प्रकार *ret_आकार);
