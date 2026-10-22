/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು NULSTR_FOREACH(i, l)                                    \
        ಗಾಗಿ (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#ವ್ಯಾಖ್ಯಾನಿಸು NULSTR_FOREACH_PAIR(i, j, l)                             \
        ಗಾಗಿ (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* nulstr_get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nulstr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool nulstr_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nulstr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಹಿಂತಿರುಗಿ nulstr_get(nulstr, needle);
}

ಅಕ್ಷರ** strv_parse_nulstr_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l, bool drop_trailing_nuls);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ** strv_parse_nulstr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l) {
        ಹಿಂತಿರುಗಿ strv_parse_nulstr_ತುಂಬಿದ(s, l, false);
}
ಅಕ್ಷರ** strv_ವಿಭಜಿಸು_nulstr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strv_from_nulstr(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nulstr) {
        ಅಕ್ಷರ **t;

        ಪ್ರತಿಜ್ಞಾ(ret);

        t = strv_ವಿಭಜಿಸು_nulstr(nulstr);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = t;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ strv_make_nulstr(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ);
ಪೂರ್ಣಾಂಕ ಗಣ_make_nulstr(ಗಣ_2 *s, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ);
