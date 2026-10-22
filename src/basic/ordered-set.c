/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

целое ordered_set_ensure_allocated(OrderedSet **s, const struct hash_ops *ops) {
        assert(s);

        если (*s)
                вернуть 0;

        *s = ordered_set_new(ops);
        если (!*s)
                вернуть -ENOMEM;

        вернуть 0;
}

целое ordered_set_ensure_put(OrderedSet **s, const struct hash_ops *ops, void *p) {
        целое r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        если (r < 0)
                вернуть r;

        вернуть ordered_set_put(*s, p);
}

целое ordered_set_consume(OrderedSet *s, void *p) {
        целое r;

        r = ordered_set_put(s, p);
        если (r <= 0)
                free(p);

        вернуть r;
}

целое ordered_set_put_strdup_full(OrderedSet **s, const struct hash_ops *hash_ops, const char *p) {
        char *c;
        целое r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        если (r < 0)
                вернуть r;

        если (ordered_set_contains(*s, p))
                вернуть 0;

        c = strdup(p);
        если (!c)
                вернуть -ENOMEM;

        вернуть ordered_set_consume(*s, c);
}

целое ordered_set_put_strdupv_full(OrderedSet **s, const struct hash_ops *hash_ops, char **l) {
        целое r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                если (r < 0)
                        вернуть r;

                ret = ret || r > 0;
        }

        вернуть ret;
}

целое ordered_set_put_string_set_full(OrderedSet **s, const struct hash_ops *hash_ops, OrderedSet *l) {
        целое r, ret = 0;
        char *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                если (r < 0)
                        вернуть r;

                ret = ret || r > 0;
        }

        вернуть ret;
}

void ordered_set_print(FILE *f, const char *field, OrderedSet *s) {
        bool space = false;
        char *p;

        assert(f);
        assert(field);

        если (ordered_set_isempty(s))
                вернуть;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
