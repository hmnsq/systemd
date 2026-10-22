/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

정수 ordered_set_ensure_allocated(OrderedSet **s, const struct hash_ops *ops) {
        assert(s);

        만약 (*s)
                반환 0;

        *s = ordered_set_new(ops);
        만약 (!*s)
                반환 -ENOMEM;

        반환 0;
}

정수 ordered_set_ensure_put(OrderedSet **s, const struct hash_ops *ops, void *p) {
        정수 r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        만약 (r < 0)
                반환 r;

        반환 ordered_set_put(*s, p);
}

정수 ordered_set_consume(OrderedSet *s, void *p) {
        정수 r;

        r = ordered_set_put(s, p);
        만약 (r <= 0)
                free(p);

        반환 r;
}

정수 ordered_set_put_strdup_full(OrderedSet **s, const struct hash_ops *hash_ops, const char *p) {
        char *c;
        정수 r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        만약 (r < 0)
                반환 r;

        만약 (ordered_set_contains(*s, p))
                반환 0;

        c = strdup(p);
        만약 (!c)
                반환 -ENOMEM;

        반환 ordered_set_consume(*s, c);
}

정수 ordered_set_put_strdupv_full(OrderedSet **s, const struct hash_ops *hash_ops, char **l) {
        정수 r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                만약 (r < 0)
                        반환 r;

                ret = ret || r > 0;
        }

        반환 ret;
}

정수 ordered_set_put_string_set_full(OrderedSet **s, const struct hash_ops *hash_ops, OrderedSet *l) {
        정수 r, ret = 0;
        char *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                만약 (r < 0)
                        반환 r;

                ret = ret || r > 0;
        }

        반환 ret;
}

void ordered_set_print(FILE *f, const char *field, OrderedSet *s) {
        bool space = false;
        char *p;

        assert(f);
        assert(field);

        만약 (ordered_set_isempty(s))
                반환;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
