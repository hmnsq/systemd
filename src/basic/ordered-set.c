/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

цяло ordered_set_ensure_allocated(OrderedSet **s, const struct hash_ops *ops) {
        assert(s);

        ако (*s)
                върни 0;

        *s = ordered_set_new(ops);
        ако (!*s)
                върни -ENOMEM;

        върни 0;
}

цяло ordered_set_ensure_put(OrderedSet **s, const struct hash_ops *ops, void *p) {
        цяло r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        ако (r < 0)
                върни r;

        върни ordered_set_put(*s, p);
}

цяло ordered_set_consume(OrderedSet *s, void *p) {
        цяло r;

        r = ordered_set_put(s, p);
        ако (r <= 0)
                free(p);

        върни r;
}

цяло ordered_set_put_strdup_full(OrderedSet **s, const struct hash_ops *hash_ops, const char *p) {
        char *c;
        цяло r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ако (r < 0)
                върни r;

        ако (ordered_set_contains(*s, p))
                върни 0;

        c = strdup(p);
        ако (!c)
                върни -ENOMEM;

        върни ordered_set_consume(*s, c);
}

цяло ordered_set_put_strdupv_full(OrderedSet **s, const struct hash_ops *hash_ops, char **l) {
        цяло r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ако (r < 0)
                        върни r;

                ret = ret || r > 0;
        }

        върни ret;
}

цяло ordered_set_put_string_set_full(OrderedSet **s, const struct hash_ops *hash_ops, OrderedSet *l) {
        цяло r, ret = 0;
        char *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ако (r < 0)
                        върни r;

                ret = ret || r > 0;
        }

        върни ret;
}

void ordered_set_print(FILE *f, const char *field, OrderedSet *s) {
        bool space = false;
        char *p;

        assert(f);
        assert(field);

        ако (ordered_set_isempty(s))
                върни;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
