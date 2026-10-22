/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "fileio.h"
#ضمن "ordered-set.h"
#ضمن "strv.h"

صحيح ordered_set_ensure_allocated(OrderedSet **s, ثابت بنية hash_ops *ops) {
        أكد(s);

        إذا (*s)
                إرجاع 0;

        *s = ordered_set_new(ops);
        إذا (!*s)
                إرجاع -ENOMEM;

        إرجاع 0;
}

صحيح ordered_set_ensure_put(OrderedSet **s, ثابت بنية hash_ops *ops, فراغ *p) {
        صحيح r;

        أكد(s);

        r = ordered_set_ensure_allocated(s, ops);
        إذا (r < 0)
                إرجاع r;

        إرجاع ordered_set_put(*s, p);
}

صحيح ordered_set_consume(OrderedSet *s, فراغ *p) {
        صحيح r;

        r = ordered_set_put(s, p);
        إذا (r <= 0)
                حرر(p);

        إرجاع r;
}

صحيح ordered_set_put_strdup_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                إرجاع r;

        إذا (ordered_set_contains(*s, p))
                إرجاع 0;

        c = strdup(p);
        إذا (!c)
                إرجاع -ENOMEM;

        إرجاع ordered_set_consume(*s, c);
}

صحيح ordered_set_put_strdupv_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح r, ret = 0;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                إذا (r < 0)
                        إرجاع r;

                ret = ret || r > 0;
        }

        إرجاع ret;
}

صحيح ordered_set_put_string_set_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, OrderedSet *l) {
        صحيح r, ret = 0;
        حرف *p;

        أكد(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                إذا (r < 0)
                        إرجاع r;

                ret = ret || r > 0;
        }

        إرجاع ret;
}

فراغ ordered_set_print(FILE *f, ثابت حرف *field, OrderedSet *s) {
        bool space = false;
        حرف *p;

        أكد(f);
        أكد(field);

        إذا (ordered_set_isempty(s))
                إرجاع;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, عدم, &space);

        fputc('\n', f);
}
