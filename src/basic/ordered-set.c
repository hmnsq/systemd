/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "ordered-set.h"
#অন্তর্ভাব "strv.h"

পূর্ণাঙ্ক ordered_set_ensure_allocated(OrderedSet **s, ধ্রুব সংরচনা hash_ops *ops) {
        প্রতিজ্ঞা(s);

        যদি (*s)
                প্রত্যাগম 0;

        *s = ordered_set_new(ops);
        যদি (!*s)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক ordered_set_ensure_put(OrderedSet **s, ধ্রুব সংরচনা hash_ops *ops, শূন্য *p) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        r = ordered_set_ensure_allocated(s, ops);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম ordered_set_put(*s, p);
}

পূর্ণাঙ্ক ordered_set_consume(OrderedSet *s, শূন্য *p) {
        পূর্ণাঙ্ক r;

        r = ordered_set_put(s, p);
        যদি (r <= 0)
                মুক্ত(p);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক ordered_set_put_strdup_full(OrderedSet **s, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব বর্ণ *p) {
        বর্ণ *c;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ordered_set_contains(*s, p))
                প্রত্যাগম 0;

        c = strdup(p);
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম ordered_set_consume(*s, c);
}

পূর্ণাঙ্ক ordered_set_put_strdupv_full(OrderedSet **s, ধ্রুব সংরচনা hash_ops *hash_ops, বর্ণ **l) {
        পূর্ণাঙ্ক r, ret = 0;

        প্রতিজ্ঞা(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                যদি (r < 0)
                        প্রত্যাগম r;

                ret = ret || r > 0;
        }

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক ordered_set_put_string_set_full(OrderedSet **s, ধ্রুব সংরচনা hash_ops *hash_ops, OrderedSet *l) {
        পূর্ণাঙ্ক r, ret = 0;
        বর্ণ *p;

        প্রতিজ্ঞা(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                যদি (r < 0)
                        প্রত্যাগম r;

                ret = ret || r > 0;
        }

        প্রত্যাগম ret;
}

শূন্য ordered_set_print(FILE *f, ধ্রুব বর্ণ *field, OrderedSet *s) {
        bool space = false;
        বর্ণ *p;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(field);

        যদি (ordered_set_isempty(s))
                প্রত্যাগম;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, রিক্ত, &space);

        fputc('\n', f);
}
