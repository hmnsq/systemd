/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

số_nguyên ordered_set_ensure_allocated(OrderedSet **s, const struct hash_ops *ops) {
        assert(s);

        nếu (*s)
                trả_về 0;

        *s = ordered_set_new(ops);
        nếu (!*s)
                trả_về -ENOMEM;

        trả_về 0;
}

số_nguyên ordered_set_ensure_put(OrderedSet **s, const struct hash_ops *ops, void *p) {
        số_nguyên r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        nếu (r < 0)
                trả_về r;

        trả_về ordered_set_put(*s, p);
}

số_nguyên ordered_set_consume(OrderedSet *s, void *p) {
        số_nguyên r;

        r = ordered_set_put(s, p);
        nếu (r <= 0)
                free(p);

        trả_về r;
}

số_nguyên ordered_set_put_strdup_full(OrderedSet **s, const struct hash_ops *hash_ops, const char *p) {
        char *c;
        số_nguyên r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        nếu (r < 0)
                trả_về r;

        nếu (ordered_set_contains(*s, p))
                trả_về 0;

        c = strdup(p);
        nếu (!c)
                trả_về -ENOMEM;

        trả_về ordered_set_consume(*s, c);
}

số_nguyên ordered_set_put_strdupv_full(OrderedSet **s, const struct hash_ops *hash_ops, char **l) {
        số_nguyên r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                nếu (r < 0)
                        trả_về r;

                ret = ret || r > 0;
        }

        trả_về ret;
}

số_nguyên ordered_set_put_string_set_full(OrderedSet **s, const struct hash_ops *hash_ops, OrderedSet *l) {
        số_nguyên r, ret = 0;
        char *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                nếu (r < 0)
                        trả_về r;

                ret = ret || r > 0;
        }

        trả_về ret;
}

void ordered_set_print(FILE *f, const char *field, OrderedSet *s) {
        bool space = false;
        char *p;

        assert(f);
        assert(field);

        nếu (ordered_set_isempty(s))
                trả_về;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
