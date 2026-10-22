/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <string.h>

#include "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        အတွက် (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        အတွက် (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

const var_na* nulstr_get(const var_na *nulstr, const var_na *needle);
static anta_hstha bool nulstr_contains(const var_na *nulstr, const var_na *needle) {
        ပြန်ပေး nulstr_get(nulstr, needle);
}

var_na** strv_parse_nulstr_full(const var_na *s, size_t l, bool drop_trailing_nuls);
static anta_hstha var_na** strv_parse_nulstr(const var_na *s, size_t l) {
        ပြန်ပေး strv_parse_nulstr_full(s, l, false);
}
var_na** strv_split_nulstr(const var_na *s);
static anta_hstha ကိန်းပြည့် strv_from_nulstr(var_na ***ret, const var_na *nulstr) {
        var_na **t;

        assert(ret);

        t = strv_split_nulstr(nulstr);
        အကယ်၍ (!t)
                ပြန်ပေး -ENOMEM;

        *ret = t;
        ပြန်ပေး 0;
}

ကိန်းပြည့် strv_make_nulstr(var_na * const *l, var_na **ret, size_t *ret_size);
ကိန်းပြည့် set_make_nulstr(Set *s, var_na **ret, size_t *ret_size);
