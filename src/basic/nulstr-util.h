/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <string.h>

#अन्तर्भाव "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        अत्थाय (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        अत्थाय (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ध्रुव वर्ण* nulstr_get(ध्रुव वर्ण *nulstr, ध्रुव वर्ण *needle);
स्थिर अन्तःस्थ bool nulstr_contains(ध्रुव वर्ण *nulstr, ध्रुव वर्ण *needle) {
        पटिनिवत्त nulstr_get(nulstr, needle);
}

वर्ण** strv_parse_nulstr_full(ध्रुव वर्ण *s, size_t l, bool drop_trailing_nuls);
स्थिर अन्तःस्थ वर्ण** strv_parse_nulstr(ध्रुव वर्ण *s, size_t l) {
        पटिनिवत्त strv_parse_nulstr_full(s, l, false);
}
वर्ण** strv_split_nulstr(ध्रुव वर्ण *s);
स्थिर अन्तःस्थ पूर्णाङ्क strv_from_nulstr(वर्ण ***ret, ध्रुव वर्ण *nulstr) {
        वर्ण **t;

        प्रतिज्ञा(ret);

        t = strv_split_nulstr(nulstr);
        सचे (!t)
                पटिनिवत्त -ENOMEM;

        *ret = t;
        पटिनिवत्त 0;
}

पूर्णाङ्क strv_make_nulstr(वर्ण * ध्रुव *l, वर्ण **ret, size_t *ret_size);
पूर्णाङ्क set_make_nulstr(Set *s, वर्ण **ret, size_t *ret_size);
