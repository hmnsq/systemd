/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

ཧྲིལ ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops) {
        assert(s);

        གལ་ཏེ (*s)
                ཕྱིར 0;

        *s = ordered_set_new(ops);
        གལ་ཏེ (!*s)
                ཕྱིར -ENOMEM;

        ཕྱིར 0;
}

ཧྲིལ ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p) {
        ཧྲིལ r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར ordered_set_put(*s, p);
}

ཧྲིལ ordered_set_consume(OrderedSet *s, void *p) {
        ཧྲིལ r;

        r = ordered_set_put(s, p);
        གལ་ཏེ (r <= 0)
                free(p);

        ཕྱིར r;
}

ཧྲིལ ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p) {
        var_na *c;
        ཧྲིལ r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ordered_set_contains(*s, p))
                ཕྱིར 0;

        c = strdup(p);
        གལ་ཏེ (!c)
                ཕྱིར -ENOMEM;

        ཕྱིར ordered_set_consume(*s, c);
}

ཧྲིལ ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        ཧྲིལ r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                ret = ret || r > 0;
        }

        ཕྱིར ret;
}

ཧྲིལ ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l) {
        ཧྲིལ r, ret = 0;
        var_na *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                ret = ret || r > 0;
        }

        ཕྱིར ret;
}

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s) {
        bool space = false;
        var_na *p;

        assert(f);
        assert(field);

        གལ་ཏེ (ordered_set_isempty(s))
                ཕྱིར;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
