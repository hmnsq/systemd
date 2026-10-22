/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <string.h>

#अन्तर्भाव "forward.h"

#परिभाषय NULSTR_FOREACH(i, l)                                    \
        प्रति (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#परिभाषय NULSTR_FOREACH_PAIR(i, j, l)                             \
        प्रति (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ध्रुव वर्ण* nulstr_get(ध्रुव वर्ण *nulstr, ध्रुव वर्ण *needle);
स्थिर अन्तःस्थ bool nulstr_अन्तर्भवति(ध्रुव वर्ण *nulstr, ध्रुव वर्ण *needle) {
        प्रत्यागम nulstr_get(nulstr, needle);
}

वर्ण** strv_parse_nulstr_पूर्णम्(ध्रुव वर्ण *s, size_t l, bool drop_trailing_nuls);
स्थिर अन्तःस्थ वर्ण** strv_parse_nulstr(ध्रुव वर्ण *s, size_t l) {
        प्रत्यागम strv_parse_nulstr_पूर्णम्(s, l, false);
}
वर्ण** strv_विभज_nulstr(ध्रुव वर्ण *s);
स्थिर अन्तःस्थ पूर्णाङ्क strv_from_nulstr(वर्ण ***ret, ध्रुव वर्ण *nulstr) {
        वर्ण **t;

        प्रतिज्ञा(ret);

        t = strv_विभज_nulstr(nulstr);
        यदि (!t)
                प्रत्यागम -ENOMEM;

        *ret = t;
        प्रत्यागम 0;
}

पूर्णाङ्क strv_make_nulstr(वर्ण * ध्रुव *l, वर्ण **ret, size_t *ret_आकार);
पूर्णाङ्क समुच्चयः_make_nulstr(समुच्चयः_2 *s, वर्ण **ret, size_t *ret_आकार);
