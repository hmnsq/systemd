/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

ကိန်းပြည့် ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops) {
        assert(s);

        အကယ်၍ (*s)
                ပြန်ပေး 0;

        *s = ordered_set_new(ops);
        အကယ်၍ (!*s)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး 0;
}

ကိန်းပြည့် ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p) {
        ကိန်းပြည့် r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး ordered_set_put(*s, p);
}

ကိန်းပြည့် ordered_set_consume(OrderedSet *s, void *p) {
        ကိန်းပြည့် r;

        r = ordered_set_put(s, p);
        အကယ်၍ (r <= 0)
                free(p);

        ပြန်ပေး r;
}

ကိန်းပြည့် ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p) {
        var_na *c;
        ကိန်းပြည့် r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (ordered_set_contains(*s, p))
                ပြန်ပေး 0;

        c = strdup(p);
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး ordered_set_consume(*s, c);
}

ကိန်းပြည့် ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        ကိန်းပြည့် r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                ret = ret || r > 0;
        }

        ပြန်ပေး ret;
}

ကိန်းပြည့် ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l) {
        ကိန်းပြည့် r, ret = 0;
        var_na *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                ret = ret || r > 0;
        }

        ပြန်ပေး ret;
}

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s) {
        bool space = false;
        var_na *p;

        assert(f);
        assert(field);

        အကယ်၍ (ordered_set_isempty(s))
                ပြန်ပေး;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
