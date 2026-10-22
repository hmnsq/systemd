/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "ordered-set.h"
#અન્તર્ભાવ "strv.h"

પૂર્ણાંક ordered_set_ensure_allocated(OrderedSet **s, અચલ સંરચના hash_ops *ops) {
        પ્રતિજ્ઞા(s);

        જો (*s)
                પાછા 0;

        *s = ordered_set_new(ops);
        જો (!*s)
                પાછા -ENOMEM;

        પાછા 0;
}

પૂર્ણાંક ordered_set_ensure_put(OrderedSet **s, અચલ સંરચના hash_ops *ops, શૂન્ય *p) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        r = ordered_set_ensure_allocated(s, ops);
        જો (r < 0)
                પાછા r;

        પાછા ordered_set_put(*s, p);
}

પૂર્ણાંક ordered_set_consume(OrderedSet *s, શૂન્ય *p) {
        પૂર્ણાંક r;

        r = ordered_set_put(s, p);
        જો (r <= 0)
                મુક્ત(p);

        પાછા r;
}

પૂર્ણાંક ordered_set_put_strdup_full(OrderedSet **s, અચલ સંરચના hash_ops *hash_ops, અચલ અક્ષર *p) {
        અક્ષર *c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        જો (r < 0)
                પાછા r;

        જો (ordered_set_contains(*s, p))
                પાછા 0;

        c = strdup(p);
        જો (!c)
                પાછા -ENOMEM;

        પાછા ordered_set_consume(*s, c);
}

પૂર્ણાંક ordered_set_put_strdupv_full(OrderedSet **s, અચલ સંરચના hash_ops *hash_ops, અક્ષર **l) {
        પૂર્ણાંક r, ret = 0;

        પ્રતિજ્ઞા(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                જો (r < 0)
                        પાછા r;

                ret = ret || r > 0;
        }

        પાછા ret;
}

પૂર્ણાંક ordered_set_put_string_set_full(OrderedSet **s, અચલ સંરચના hash_ops *hash_ops, OrderedSet *l) {
        પૂર્ણાંક r, ret = 0;
        અક્ષર *p;

        પ્રતિજ્ઞા(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                જો (r < 0)
                        પાછા r;

                ret = ret || r > 0;
        }

        પાછા ret;
}

શૂન્ય ordered_set_print(FILE *f, અચલ અક્ષર *field, OrderedSet *s) {
        bool space = false;
        અક્ષર *p;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(field);

        જો (ordered_set_isempty(s))
                પાછા;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, ખાલી, &space);

        fputc('\n', f);
}
