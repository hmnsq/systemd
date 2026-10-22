/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "fileio.h"
#ضمن "ordered-set.h"
#ضمن "strv.h"

صحيح ordered_set_ensure_allocated(OrderedSet **s, ثابت بنية hash_ops *ops) {
        أكد(s);

        ܐܢ (*s)
                ܦܢܐ 0;

        *s = ordered_set_new(ops);
        ܐܢ (!*s)
                ܦܢܐ -ENOMEM;

        ܦܢܐ 0;
}

صحيح ordered_set_ensure_put(OrderedSet **s, ثابت بنية hash_ops *ops, فراغ *p) {
        صحيح r;

        أكد(s);

        r = ordered_set_ensure_allocated(s, ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ ordered_set_put(*s, p);
}

صحيح ordered_set_consume(OrderedSet *s, فراغ *p) {
        صحيح r;

        r = ordered_set_put(s, p);
        ܐܢ (r <= 0)
                حرر(p);

        ܦܢܐ r;
}

صحيح ordered_set_put_strdup_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ordered_set_contains(*s, p))
                ܦܢܐ 0;

        c = strdup(p);
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;

        ܦܢܐ ordered_set_consume(*s, c);
}

صحيح ordered_set_put_strdupv_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح r, ret = 0;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ret = ret || r > 0;
        }

        ܦܢܐ ret;
}

صحيح ordered_set_put_string_set_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, OrderedSet *l) {
        صحيح r, ret = 0;
        حرف *p;

        أكد(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ret = ret || r > 0;
        }

        ܦܢܐ ret;
}

فراغ ordered_set_print(FILE *f, ثابت حرف *field, OrderedSet *s) {
        bool space = false;
        حرف *p;

        أكد(f);
        أكد(field);

        ܐܢ (ordered_set_isempty(s))
                ܦܢܐ;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, عدم, &space);

        fputc('\n', f);
}
