/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        માટે (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        માટે (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

અચલ અક્ષર* nulstr_get(અચલ અક્ષર *nulstr, અચલ અક્ષર *needle);
સ્થિર અંતઃરેખ bool nulstr_contains(અચલ અક્ષર *nulstr, અચલ અક્ષર *needle) {
        પાછા nulstr_get(nulstr, needle);
}

અક્ષર** strv_parse_nulstr_full(અચલ અક્ષર *s, size_t l, bool drop_trailing_nuls);
સ્થિર અંતઃરેખ અક્ષર** strv_parse_nulstr(અચલ અક્ષર *s, size_t l) {
        પાછા strv_parse_nulstr_full(s, l, false);
}
અક્ષર** strv_split_nulstr(અચલ અક્ષર *s);
સ્થિર અંતઃરેખ પૂર્ણાંક strv_from_nulstr(અક્ષર ***ret, અચલ અક્ષર *nulstr) {
        અક્ષર **t;

        પ્રતિજ્ઞા(ret);

        t = strv_split_nulstr(nulstr);
        જો (!t)
                પાછા -ENOMEM;

        *ret = t;
        પાછા 0;
}

પૂર્ણાંક strv_make_nulstr(અક્ષર * અચલ *l, અક્ષર **ret, size_t *ret_size);
પૂર્ણાંક set_make_nulstr(Set *s, અક્ષર **ret, size_t *ret_size);
