/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NULSTR_FOREACH(i, l)                                    \
        ਲਈ (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NULSTR_FOREACH_PAIR(i, j, l)                             \
        ਲਈ (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

ਅਚਲ ਅੱਖਰ* nulstr_get(ਅਚਲ ਅੱਖਰ *nulstr, ਅਚਲ ਅੱਖਰ *needle);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool nulstr_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਅੱਖਰ *nulstr, ਅਚਲ ਅੱਖਰ *needle) {
        ਵਾਪਸ nulstr_get(nulstr, needle);
}

ਅੱਖਰ** strv_parse_nulstr_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, size_t l, bool drop_trailing_nuls);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ** strv_parse_nulstr(ਅਚਲ ਅੱਖਰ *s, size_t l) {
        ਵਾਪਸ strv_parse_nulstr_ਭਰਿਆ(s, l, false);
}
ਅੱਖਰ** strv_ਵੰਡੋ_nulstr(ਅਚਲ ਅੱਖਰ *s);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strv_from_nulstr(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *nulstr) {
        ਅੱਖਰ **t;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        t = strv_ਵੰਡੋ_nulstr(nulstr);
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        *ret = t;
        ਵਾਪਸ 0;
}

ਪੂਰਨ strv_make_nulstr(ਅੱਖਰ * ਅਚਲ *l, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ);
ਪੂਰਨ ਸੈੱਟ_make_nulstr(ਸੈੱਟ_2 *s, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ);
