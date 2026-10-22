/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "ordered-set.h"
#অন্তর্ভাব "strv.h"

পূর্ণসংখ্যা ordered_set_ensure_allocated(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *ops) {
        প্রতিজ্ঞা(s);

        যদি (*s)
                ফেরত 0;

        *s = ordered_set_new(ops);
        যদি (!*s)
                ফেরত -ENOMEM;

        ফেরত 0;
}

পূর্ণসংখ্যা ordered_set_ensure_put(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *ops, শূন্য *p) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        r = ordered_set_ensure_allocated(s, ops);
        যদি (r < 0)
                ফেরত r;

        ফেরত ordered_set_put(*s, p);
}

পূর্ণসংখ্যা ordered_set_consume(OrderedSet *s, শূন্য *p) {
        পূর্ণসংখ্যা r;

        r = ordered_set_put(s, p);
        যদি (r <= 0)
                মুক্ত(p);

        ফেরত r;
}

পূর্ণসংখ্যা ordered_set_put_strdup_full(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক অক্ষর *p) {
        অক্ষর *c;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        যদি (r < 0)
                ফেরত r;

        যদি (ordered_set_contains(*s, p))
                ফেরত 0;

        c = strdup(p);
        যদি (!c)
                ফেরত -ENOMEM;

        ফেরত ordered_set_consume(*s, c);
}

পূর্ণসংখ্যা ordered_set_put_strdupv_full(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *hash_ops, অক্ষর **l) {
        পূর্ণসংখ্যা r, ret = 0;

        প্রতিজ্ঞা(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                যদি (r < 0)
                        ফেরত r;

                ret = ret || r > 0;
        }

        ফেরত ret;
}

পূর্ণসংখ্যা ordered_set_put_string_set_full(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *hash_ops, OrderedSet *l) {
        পূর্ণসংখ্যা r, ret = 0;
        অক্ষর *p;

        প্রতিজ্ঞা(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                যদি (r < 0)
                        ফেরত r;

                ret = ret || r > 0;
        }

        ফেরত ret;
}

শূন্য ordered_set_print(FILE *f, ধ্রুবক অক্ষর *field, OrderedSet *s) {
        bool space = false;
        অক্ষর *p;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(field);

        যদি (ordered_set_isempty(s))
                ফেরত;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, ফাঁকা, &space);

        fputc('\n', f);
}
