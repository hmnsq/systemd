/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "fileio.h"
#अन्तर्भाव "ordered-set.h"
#अन्तर्भाव "strv.h"

पूर्णांक ordered_set_ensure_allocated(OrderedSet **s, स्थिरांक संरचना hash_ops *ops) {
        प्रतिज्ञा(s);

        जर (*s)
                परत 0;

        *s = ordered_set_new(ops);
        जर (!*s)
                परत -ENOMEM;

        परत 0;
}

पूर्णांक ordered_set_ensure_put(OrderedSet **s, स्थिरांक संरचना hash_ops *ops, शून्य *p) {
        पूर्णांक r;

        प्रतिज्ञा(s);

        r = ordered_set_ensure_allocated(s, ops);
        जर (r < 0)
                परत r;

        परत ordered_set_put(*s, p);
}

पूर्णांक ordered_set_consume(OrderedSet *s, शून्य *p) {
        पूर्णांक r;

        r = ordered_set_put(s, p);
        जर (r <= 0)
                मुक्त(p);

        परत r;
}

पूर्णांक ordered_set_put_strdup_full(OrderedSet **s, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक अक्षर *p) {
        अक्षर *c;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        जर (r < 0)
                परत r;

        जर (ordered_set_contains(*s, p))
                परत 0;

        c = strdup(p);
        जर (!c)
                परत -ENOMEM;

        परत ordered_set_consume(*s, c);
}

पूर्णांक ordered_set_put_strdupv_full(OrderedSet **s, स्थिरांक संरचना hash_ops *hash_ops, अक्षर **l) {
        पूर्णांक r, ret = 0;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                जर (r < 0)
                        परत r;

                ret = ret || r > 0;
        }

        परत ret;
}

पूर्णांक ordered_set_put_string_set_full(OrderedSet **s, स्थिरांक संरचना hash_ops *hash_ops, OrderedSet *l) {
        पूर्णांक r, ret = 0;
        अक्षर *p;

        प्रतिज्ञा(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                जर (r < 0)
                        परत r;

                ret = ret || r > 0;
        }

        परत ret;
}

शून्य ordered_set_print(खाता *f, स्थिरांक अक्षर *field, OrderedSet *s) {
        bool space = false;
        अक्षर *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(field);

        जर (ordered_set_isempty(s))
                परत;

        ख_माला_दो(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, रिक्त, &space);

        ख_अक्षर_दो('\n', f);
}
