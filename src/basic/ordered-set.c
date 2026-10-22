/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

ціле ordered_set_ensure_allocated(OrderedSet **s, const struct hash_ops *ops) {
        assert(s);

        якщо (*s)
                повернути 0;

        *s = ordered_set_new(ops);
        якщо (!*s)
                повернути -ENOMEM;

        повернути 0;
}

ціле ordered_set_ensure_put(OrderedSet **s, const struct hash_ops *ops, void *p) {
        ціле r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        якщо (r < 0)
                повернути r;

        повернути ordered_set_put(*s, p);
}

ціле ordered_set_consume(OrderedSet *s, void *p) {
        ціле r;

        r = ordered_set_put(s, p);
        якщо (r <= 0)
                free(p);

        повернути r;
}

ціле ordered_set_put_strdup_full(OrderedSet **s, const struct hash_ops *hash_ops, const char *p) {
        char *c;
        ціле r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        якщо (r < 0)
                повернути r;

        якщо (ordered_set_contains(*s, p))
                повернути 0;

        c = strdup(p);
        якщо (!c)
                повернути -ENOMEM;

        повернути ordered_set_consume(*s, c);
}

ціле ordered_set_put_strdupv_full(OrderedSet **s, const struct hash_ops *hash_ops, char **l) {
        ціле r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                якщо (r < 0)
                        повернути r;

                ret = ret || r > 0;
        }

        повернути ret;
}

ціле ordered_set_put_string_set_full(OrderedSet **s, const struct hash_ops *hash_ops, OrderedSet *l) {
        ціле r, ret = 0;
        char *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                якщо (r < 0)
                        повернути r;

                ret = ret || r > 0;
        }

        повернути ret;
}

void ordered_set_print(FILE *f, const char *field, OrderedSet *s) {
        bool space = false;
        char *p;

        assert(f);
        assert(field);

        якщо (ordered_set_isempty(s))
                повернути;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
