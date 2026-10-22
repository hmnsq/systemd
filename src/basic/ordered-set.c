/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "ordered-set.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಪೂರ್ಣಾಙ್ಕ ordered_set_ensure_allocated(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *ops) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (*s)
                ಪ್ರತ್ಯಾಗಮ 0;

        *s = ordered_set_new(ops);
        ಯದಿ (!*s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_ensure_put(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *ops, ಶೂನ್ಯ *p) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ordered_set_ensure_allocated(s, ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ ordered_set_put(*s, p);
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_consume(OrderedSet *s, ಶೂನ್ಯ *p) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = ordered_set_put(s, p);
        ಯದಿ (r <= 0)
                ಮುಕ್ತ(p);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_put_strdup_full(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ವರ್ಣ *p) {
        ವರ್ಣ *c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ordered_set_contains(*s, p))
                ಪ್ರತ್ಯಾಗಮ 0;

        c = strdup(p);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ ordered_set_consume(*s, c);
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_put_strdupv_full(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ವರ್ಣ **l) {
        ಪೂರ್ಣಾಙ್ಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ret = ret || r > 0;
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_put_string_set_full(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, OrderedSet *l) {
        ಪೂರ್ಣಾಙ್ಕ r, ret = 0;
        ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ret = ret || r > 0;
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ ordered_set_print(FILE *f, ಧ್ರುವ ವರ್ಣ *field, OrderedSet *s) {
        bool space = false;
        ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(field);

        ಯದಿ (ordered_set_isempty(s))
                ಪ್ರತ್ಯಾಗಮ;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, ರಿಕ್ತ, &space);

        fputc('\n', f);
}
