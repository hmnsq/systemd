/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

ចំនួនគត់ ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops) {
        assert(s);

        ប្រសិនបើ (*s)
                ត្រឡប់ 0;

        *s = ordered_set_new(ops);
        ប្រសិនបើ (!*s)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ 0;
}

ចំនួនគត់ ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p) {
        ចំនួនគត់ r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ ordered_set_put(*s, p);
}

ចំនួនគត់ ordered_set_consume(OrderedSet *s, void *p) {
        ចំនួនគត់ r;

        r = ordered_set_put(s, p);
        ប្រសិនបើ (r <= 0)
                free(p);

        ត្រឡប់ r;
}

ចំនួនគត់ ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p) {
        var_na *c;
        ចំនួនគត់ r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (ordered_set_contains(*s, p))
                ត្រឡប់ 0;

        c = strdup(p);
        ប្រសិនបើ (!c)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ ordered_set_consume(*s, c);
}

ចំនួនគត់ ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        ចំនួនគត់ r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ret = ret || r > 0;
        }

        ត្រឡប់ ret;
}

ចំនួនគត់ ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l) {
        ចំនួនគត់ r, ret = 0;
        var_na *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ret = ret || r > 0;
        }

        ត្រឡប់ ret;
}

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s) {
        bool space = false;
        var_na *p;

        assert(f);
        assert(field);

        ប្រសិនបើ (ordered_set_isempty(s))
                ត្រឡប់;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
