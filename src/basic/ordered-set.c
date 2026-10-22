/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "fileio.h"
#अन्तर्भाव "ordered-set.h"
#अन्तर्भाव "strv.h"

पूर्णाङ्क ordered_समुच्चयः_ensure_allocated(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *ops) {
        प्रतिज्ञा(s);

        यदि (*s)
                प्रत्यागम 0;

        *s = ordered_समुच्चयः_new(ops);
        यदि (!*s)
                प्रत्यागम -ENOMEM;

        प्रत्यागम 0;
}

पूर्णाङ्क ordered_समुच्चयः_ensure_put(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *ops, शून्य *p) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = ordered_समुच्चयः_ensure_allocated(s, ops);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम ordered_समुच्चयः_put(*s, p);
}

पूर्णाङ्क ordered_समुच्चयः_consume(Ordered_समुच्चयः *s, शून्य *p) {
        पूर्णाङ्क r;

        r = ordered_समुच्चयः_put(s, p);
        यदि (r <= 0)
                मुक्त(p);

        प्रत्यागम r;
}

पूर्णाङ्क ordered_समुच्चयः_put_strdup_पूर्णम्(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = ordered_समुच्चयः_ensure_allocated(s, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ordered_समुच्चयः_अन्तर्भवति(*s, p))
                प्रत्यागम 0;

        c = strdup(p);
        यदि (!c)
                प्रत्यागम -ENOMEM;

        प्रत्यागम ordered_समुच्चयः_consume(*s, c);
}

पूर्णाङ्क ordered_समुच्चयः_put_strdupv_पूर्णम्(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = ordered_समुच्चयः_put_strdup_पूर्णम्(s, hash_ops, *i);
                यदि (r < 0)
                        प्रत्यागम r;

                ret = ret || r > 0;
        }

        प्रत्यागम ret;
}

पूर्णाङ्क ordered_समुच्चयः_put_string_समुच्चयः_पूर्णम्(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *hash_ops, Ordered_समुच्चयः *l) {
        पूर्णाङ्क r, ret = 0;
        वर्ण *p;

        प्रतिज्ञा(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_समुच्चयः_FOREACH(p, l) {
                r = ordered_समुच्चयः_put_strdup_पूर्णम्(s, hash_ops, p);
                यदि (r < 0)
                        प्रत्यागम r;

                ret = ret || r > 0;
        }

        प्रत्यागम ret;
}

शून्य ordered_समुच्चयः_print(FILE *f, ध्रुव वर्ण *क्षेत्रम्, Ordered_समुच्चयः *s) {
        bool space = false;
        वर्ण *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(क्षेत्रम्);

        यदि (ordered_समुच्चयः_isempty(s))
                प्रत्यागम;

        fputs(क्षेत्रम्, f);

        ORDERED_समुच्चयः_FOREACH(p, s)
                fputs_with_separator(f, p, रिक्त, &space);

        fputc('\n', f);
}
