/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "fileio.h"
#अन्तर्भाव "ordered-set.h"
#अन्तर्भाव "strv.h"

पूर्णाङ्क ordered_set_ensure_allocated(OrderedSet **s, ध्रुव संरचना hash_ops *ops) {
        प्रतिज्ञा(s);

        जइ (*s)
                प्रत्यागम 0;

        *s = ordered_set_new(ops);
        जइ (!*s)
                प्रत्यागम -ENOMEM;

        प्रत्यागम 0;
}

पूर्णाङ्क ordered_set_ensure_put(OrderedSet **s, ध्रुव संरचना hash_ops *ops, शून्य *p) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = ordered_set_ensure_allocated(s, ops);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम ordered_set_put(*s, p);
}

पूर्णाङ्क ordered_set_consume(OrderedSet *s, शून्य *p) {
        पूर्णाङ्क r;

        r = ordered_set_put(s, p);
        जइ (r <= 0)
                मुक्त(p);

        प्रत्यागम r;
}

पूर्णाङ्क ordered_set_put_strdup_full(OrderedSet **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (ordered_set_contains(*s, p))
                प्रत्यागम 0;

        c = strdup(p);
        जइ (!c)
                प्रत्यागम -ENOMEM;

        प्रत्यागम ordered_set_consume(*s, c);
}

पूर्णाङ्क ordered_set_put_strdupv_full(OrderedSet **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                जइ (r < 0)
                        प्रत्यागम r;

                ret = ret || r > 0;
        }

        प्रत्यागम ret;
}

पूर्णाङ्क ordered_set_put_string_set_full(OrderedSet **s, ध्रुव संरचना hash_ops *hash_ops, OrderedSet *l) {
        पूर्णाङ्क r, ret = 0;
        वर्ण *p;

        प्रतिज्ञा(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                जइ (r < 0)
                        प्रत्यागम r;

                ret = ret || r > 0;
        }

        प्रत्यागम ret;
}

शून्य ordered_set_print(FILE *f, ध्रुव वर्ण *field, OrderedSet *s) {
        bool space = false;
        वर्ण *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(field);

        जइ (ordered_set_isempty(s))
                प्रत्यागम;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, रिक्त, &space);

        fputc('\n', f);
}
