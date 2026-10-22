/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <string.h>

#अन्तर्भाव "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        लागि (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = म_अक्षर((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        लागि (typeof(*(l)) *(i) = (l), *(j) = म_अक्षर((i), 0)+1; (i) && *(i); (i) = म_अक्षर((j), 0)+1, (j) = *(i) ? म_अक्षर((i), 0)+1 : (i))

अचर वर्ण* nulstr_get(अचर वर्ण *nulstr, अचर वर्ण *needle);
स्थिर अन्तःस्थ bool nulstr_contains(अचर वर्ण *nulstr, अचर वर्ण *needle) {
        फर्काऊ nulstr_get(nulstr, needle);
}

वर्ण** strv_parse_nulstr_full(अचर वर्ण *s, माप_प्रकार l, bool drop_trailing_nuls);
स्थिर अन्तःस्थ वर्ण** strv_parse_nulstr(अचर वर्ण *s, माप_प्रकार l) {
        फर्काऊ strv_parse_nulstr_full(s, l, false);
}
वर्ण** strv_split_nulstr(अचर वर्ण *s);
स्थिर अन्तःस्थ पूर्णाङ्क strv_from_nulstr(वर्ण ***ret, अचर वर्ण *nulstr) {
        वर्ण **t;

        प्रतिज्ञा(ret);

        t = strv_split_nulstr(nulstr);
        यदि (!t)
                फर्काऊ -ENOMEM;

        *ret = t;
        फर्काऊ 0;
}

पूर्णाङ्क strv_make_nulstr(वर्ण * अचर *l, वर्ण **ret, माप_प्रकार *ret_size);
पूर्णाङ्क set_make_nulstr(Set *s, वर्ण **ret, माप_प्रकार *ret_size);
