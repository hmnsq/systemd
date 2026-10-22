/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "fileio.h"
#अन्तर्भाव "ordered-set.h"
#अन्तर्भाव "strv.h"

पूर्णाङ्क ordered_set_ensure_allocated(OrderedSet **s, अचर संरचना hash_ops *ops) {
        प्रतिज्ञा(s);

        यदि (*s)
                फर्काऊ 0;

        *s = ordered_set_new(ops);
        यदि (!*s)
                फर्काऊ -ENOMEM;

        फर्काऊ 0;
}

पूर्णाङ्क ordered_set_ensure_put(OrderedSet **s, अचर संरचना hash_ops *ops, शून्य *p) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = ordered_set_ensure_allocated(s, ops);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ ordered_set_put(*s, p);
}

पूर्णाङ्क ordered_set_consume(OrderedSet *s, शून्य *p) {
        पूर्णाङ्क r;

        r = ordered_set_put(s, p);
        यदि (r <= 0)
                मुक्त(p);

        फर्काऊ r;
}

पूर्णाङ्क ordered_set_put_strdup_full(OrderedSet **s, अचर संरचना hash_ops *hash_ops, अचर वर्ण *p) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        यदि (ordered_set_contains(*s, p))
                फर्काऊ 0;

        c = strdup(p);
        यदि (!c)
                फर्काऊ -ENOMEM;

        फर्काऊ ordered_set_consume(*s, c);
}

पूर्णाङ्क ordered_set_put_strdupv_full(OrderedSet **s, अचर संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                यदि (r < 0)
                        फर्काऊ r;

                ret = ret || r > 0;
        }

        फर्काऊ ret;
}

पूर्णाङ्क ordered_set_put_string_set_full(OrderedSet **s, अचर संरचना hash_ops *hash_ops, OrderedSet *l) {
        पूर्णाङ्क r, ret = 0;
        वर्ण *p;

        प्रतिज्ञा(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                यदि (r < 0)
                        फर्काऊ r;

                ret = ret || r > 0;
        }

        फर्काऊ ret;
}

शून्य ordered_set_print(खाता *f, अचर वर्ण *field, OrderedSet *s) {
        bool space = false;
        वर्ण *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(field);

        यदि (ordered_set_isempty(s))
                फर्काऊ;

        ख_माला_दो(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, रिक्त, &space);

        ख_अक्षर_दो('\n', f);
}
