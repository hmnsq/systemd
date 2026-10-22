/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

จำนวนเต็ม ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops) {
        assert(s);

        ถ้า (*s)
                คืน 0;

        *s = ordered_set_new(ops);
        ถ้า (!*s)
                คืน -ENOMEM;

        คืน 0;
}

จำนวนเต็ม ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p) {
        จำนวนเต็ม r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        ถ้า (r < 0)
                คืน r;

        คืน ordered_set_put(*s, p);
}

จำนวนเต็ม ordered_set_consume(OrderedSet *s, void *p) {
        จำนวนเต็ม r;

        r = ordered_set_put(s, p);
        ถ้า (r <= 0)
                free(p);

        คืน r;
}

จำนวนเต็ม ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p) {
        var_na *c;
        จำนวนเต็ม r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ordered_set_contains(*s, p))
                คืน 0;

        c = strdup(p);
        ถ้า (!c)
                คืน -ENOMEM;

        คืน ordered_set_consume(*s, c);
}

จำนวนเต็ม ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        จำนวนเต็ม r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ถ้า (r < 0)
                        คืน r;

                ret = ret || r > 0;
        }

        คืน ret;
}

จำนวนเต็ม ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l) {
        จำนวนเต็ม r, ret = 0;
        var_na *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ถ้า (r < 0)
                        คืน r;

                ret = ret || r > 0;
        }

        คืน ret;
}

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s) {
        bool space = false;
        var_na *p;

        assert(f);
        assert(field);

        ถ้า (ordered_set_isempty(s))
                คืน;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
