/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        വേണ്ടി (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        വേണ്ടി (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

സ്ഥിരം അക്ഷരം* nulstr_get(സ്ഥിരം അക്ഷരം *nulstr, സ്ഥിരം അക്ഷരം *needle);
സ്ഥിതം വരിയിൽ bool nulstr_contains(സ്ഥിരം അക്ഷരം *nulstr, സ്ഥിരം അക്ഷരം *needle) {
        തിരികെ nulstr_get(nulstr, needle);
}

അക്ഷരം** strv_parse_nulstr_full(സ്ഥിരം അക്ഷരം *s, size_t l, bool drop_trailing_nuls);
സ്ഥിതം വരിയിൽ അക്ഷരം** strv_parse_nulstr(സ്ഥിരം അക്ഷരം *s, size_t l) {
        തിരികെ strv_parse_nulstr_full(s, l, false);
}
അക്ഷരം** strv_split_nulstr(സ്ഥിരം അക്ഷരം *s);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strv_from_nulstr(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *nulstr) {
        അക്ഷരം **t;

        പ്രതിജ്ഞാ(ret);

        t = strv_split_nulstr(nulstr);
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        *ret = t;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ strv_make_nulstr(അക്ഷരം * സ്ഥിരം *l, അക്ഷരം **ret, size_t *ret_size);
പൂർണ്ണസംഖ്യ set_make_nulstr(Set *s, അക്ഷരം **ret, size_t *ret_size);
