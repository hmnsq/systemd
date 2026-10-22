/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "fileio.h"
#ضمن "ordered-set.h"
#ضمن "strv.h"

صحيح ordered_set_ensure_allocated(OrderedSet **s, ثابت بنية hash_ops *ops) {
        أكد(s);

        𐡄𐡍 (*s)
                أرجع 0;

        *s = ordered_set_new(ops);
        𐡄𐡍 (!*s)
                أرجع -ENOMEM;

        أرجع 0;
}

صحيح ordered_set_ensure_put(OrderedSet **s, ثابت بنية hash_ops *ops, فراغ *p) {
        صحيح r;

        أكد(s);

        r = ordered_set_ensure_allocated(s, ops);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع ordered_set_put(*s, p);
}

صحيح ordered_set_consume(OrderedSet *s, فراغ *p) {
        صحيح r;

        r = ordered_set_put(s, p);
        𐡄𐡍 (r <= 0)
                حرر(p);

        أرجع r;
}

صحيح ordered_set_put_strdup_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (ordered_set_contains(*s, p))
                أرجع 0;

        c = strdup(p);
        𐡄𐡍 (!c)
                أرجع -ENOMEM;

        أرجع ordered_set_consume(*s, c);
}

صحيح ordered_set_put_strdupv_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح r, ret = 0;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                𐡄𐡍 (r < 0)
                        أرجع r;

                ret = ret || r > 0;
        }

        أرجع ret;
}

صحيح ordered_set_put_string_set_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, OrderedSet *l) {
        صحيح r, ret = 0;
        حرف *p;

        أكد(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                𐡄𐡍 (r < 0)
                        أرجع r;

                ret = ret || r > 0;
        }

        أرجع ret;
}

فراغ ordered_set_print(FILE *f, ثابت حرف *field, OrderedSet *s) {
        bool space = false;
        حرف *p;

        أكد(f);
        أكد(field);

        𐡄𐡍 (ordered_set_isempty(s))
                أرجع;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, عدم, &space);

        fputc('\n', f);
}
