/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "fileio.h"
#अन्तर्भाव "ordered-set.h"
#अन्तर्भाव "strv.h"

पूर्णाङ्क ordered_set_ensure_allocated(OrderedSet **s, ध्रुव संरचना hash_ops *ops) {
        प्रतिज्ञा(s);

        सचे (*s)
                पटिनिवत्त 0;

        *s = ordered_set_new(ops);
        सचे (!*s)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त 0;
}

पूर्णाङ्क ordered_set_ensure_put(OrderedSet **s, ध्रुव संरचना hash_ops *ops, शून्य *p) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = ordered_set_ensure_allocated(s, ops);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त ordered_set_put(*s, p);
}

पूर्णाङ्क ordered_set_consume(OrderedSet *s, शून्य *p) {
        पूर्णाङ्क r;

        r = ordered_set_put(s, p);
        सचे (r <= 0)
                मुक्त(p);

        पटिनिवत्त r;
}

पूर्णाङ्क ordered_set_put_strdup_full(OrderedSet **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ordered_set_contains(*s, p))
                पटिनिवत्त 0;

        c = strdup(p);
        सचे (!c)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त ordered_set_consume(*s, c);
}

पूर्णाङ्क ordered_set_put_strdupv_full(OrderedSet **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                सचे (r < 0)
                        पटिनिवत्त r;

                ret = ret || r > 0;
        }

        पटिनिवत्त ret;
}

पूर्णाङ्क ordered_set_put_string_set_full(OrderedSet **s, ध्रुव संरचना hash_ops *hash_ops, OrderedSet *l) {
        पूर्णाङ्क r, ret = 0;
        वर्ण *p;

        प्रतिज्ञा(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                सचे (r < 0)
                        पटिनिवत्त r;

                ret = ret || r > 0;
        }

        पटिनिवत्त ret;
}

शून्य ordered_set_print(FILE *f, ध्रुव वर्ण *field, OrderedSet *s) {
        bool space = false;
        वर्ण *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(field);

        सचे (ordered_set_isempty(s))
                पटिनिवत्त;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, रिक्त, &space);

        fputc('\n', f);
}
