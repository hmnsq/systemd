/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <string.h>

#include "forward.h"

#define NULSTR_FOREACH(i, l)                                    \
        위해 (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        위해 (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

const char* nulstr_get(const char *nulstr, const char *needle);
static inline bool nulstr_contains(const char *nulstr, const char *needle) {
        반환 nulstr_get(nulstr, needle);
}

char** strv_parse_nulstr_full(const char *s, size_t l, bool drop_trailing_nuls);
static inline char** strv_parse_nulstr(const char *s, size_t l) {
        반환 strv_parse_nulstr_full(s, l, false);
}
char** strv_split_nulstr(const char *s);
static inline 정수 strv_from_nulstr(char ***ret, const char *nulstr) {
        char **t;

        assert(ret);

        t = strv_split_nulstr(nulstr);
        만약 (!t)
                반환 -ENOMEM;

        *ret = t;
        반환 0;
}

정수 strv_make_nulstr(char * const *l, char **ret, size_t *ret_size);
정수 set_make_nulstr(Set *s, char **ret, size_t *ret_size);
