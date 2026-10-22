/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "ordered-set.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_ensure_allocated(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *ops) {
        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (*s)
                ଫେରନ୍ତୁ 0;

        *s = ordered_set_new(ops);
        ଯଦି (!*s)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_ensure_put(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *ops, ଶୂନ୍ୟ *p) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        r = ordered_set_ensure_allocated(s, ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ ordered_set_put(*s, p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_consume(OrderedSet *s, ଶୂନ୍ୟ *p) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = ordered_set_put(s, p);
        ଯଦି (r <= 0)
                ମୁକ୍ତ(p);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put_strdup_full(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଅକ୍ଷର *p) {
        ଅକ୍ଷର *c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ordered_set_contains(*s, p))
                ଫେରନ୍ତୁ 0;

        c = strdup(p);
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ ordered_set_consume(*s, c);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put_strdupv_full(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅକ୍ଷର **l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;

        ପ୍ରତିଜ୍ଞା(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ret = ret || r > 0;
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put_string_set_full(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, OrderedSet *l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;
        ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ret = ret || r > 0;
        }

        ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ ordered_set_print(FILE *f, ଅଚଳ ଅକ୍ଷର *field, OrderedSet *s) {
        bool space = false;
        ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(field);

        ଯଦି (ordered_set_isempty(s))
                ଫେରନ୍ତୁ;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, ଖାଲି, &space);

        fputc('\n', f);
}
