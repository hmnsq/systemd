/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "ordered-set.h"
#অন্তর্ভাৱ "strv.h"

পূর্ণাঙ্ক ordered_set_ensure_allocated(OrderedSet **s, ধ্রুৱ সংরচনা hash_ops *ops) {
        প্রতিজ্ঞা(s);

        যদি (*s)
                ঘূৰাই 0;

        *s = ordered_set_new(ops);
        যদি (!*s)
                ঘূৰাই -ENOMEM;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক ordered_set_ensure_put(OrderedSet **s, ধ্রুৱ সংরচনা hash_ops *ops, শূন্য *p) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        r = ordered_set_ensure_allocated(s, ops);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই ordered_set_put(*s, p);
}

পূর্ণাঙ্ক ordered_set_consume(OrderedSet *s, শূন্য *p) {
        পূর্ণাঙ্ক r;

        r = ordered_set_put(s, p);
        যদি (r <= 0)
                মুক্ত(p);

        ঘূৰাই r;
}

পূর্ণাঙ্ক ordered_set_put_strdup_full(OrderedSet **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, ধ্রুৱ ৱর্ণ *p) {
        ৱর্ণ *c;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ordered_set_contains(*s, p))
                ঘূৰাই 0;

        c = strdup(p);
        যদি (!c)
                ঘূৰাই -ENOMEM;

        ঘূৰাই ordered_set_consume(*s, c);
}

পূর্ণাঙ্ক ordered_set_put_strdupv_full(OrderedSet **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, ৱর্ণ **l) {
        পূর্ণাঙ্ক r, ret = 0;

        প্রতিজ্ঞা(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                যদি (r < 0)
                        ঘূৰাই r;

                ret = ret || r > 0;
        }

        ঘূৰাই ret;
}

পূর্ণাঙ্ক ordered_set_put_string_set_full(OrderedSet **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, OrderedSet *l) {
        পূর্ণাঙ্ক r, ret = 0;
        ৱর্ণ *p;

        প্রতিজ্ঞা(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                যদি (r < 0)
                        ঘূৰাই r;

                ret = ret || r > 0;
        }

        ঘূৰাই ret;
}

শূন্য ordered_set_print(FILE *f, ধ্রুৱ ৱর্ণ *field, OrderedSet *s) {
        bool space = false;
        ৱর্ণ *p;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(field);

        যদি (ordered_set_isempty(s))
                ঘূৰাই;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, রিক্ত, &space);

        fputc('\n', f);
}
