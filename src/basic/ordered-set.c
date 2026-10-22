/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>

#समावेश "fileio.h"
#समावेश "ordered-set.h"
#समावेश "strv.h"

पूर्णांक ordered_set_ensure_allocated(OrderedSet **s, स्थिर काष्ठा hash_ops *ops) {
        निश्चित(s);

        अगर (*s)
                वापस 0;

        *s = ordered_set_new(ops);
        अगर (!*s)
                वापस -ENOMEM;

        वापस 0;
}

पूर्णांक ordered_set_ensure_put(OrderedSet **s, स्थिर काष्ठा hash_ops *ops, व्योम *p) {
        पूर्णांक r;

        निश्चित(s);

        r = ordered_set_ensure_allocated(s, ops);
        अगर (r < 0)
                वापस r;

        वापस ordered_set_put(*s, p);
}

पूर्णांक ordered_set_consume(OrderedSet *s, व्योम *p) {
        पूर्णांक r;

        r = ordered_set_put(s, p);
        अगर (r <= 0)
                मुक्त(p);

        वापस r;
}

पूर्णांक ordered_set_put_strdup_full(OrderedSet **s, स्थिर काष्ठा hash_ops *hash_ops, स्थिर अक्षर *p) {
        अक्षर *c;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        अगर (r < 0)
                वापस r;

        अगर (ordered_set_contains(*s, p))
                वापस 0;

        c = strdup(p);
        अगर (!c)
                वापस -ENOMEM;

        वापस ordered_set_consume(*s, c);
}

पूर्णांक ordered_set_put_strdupv_full(OrderedSet **s, स्थिर काष्ठा hash_ops *hash_ops, अक्षर **l) {
        पूर्णांक r, ret = 0;

        निश्चित(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                अगर (r < 0)
                        वापस r;

                ret = ret || r > 0;
        }

        वापस ret;
}

पूर्णांक ordered_set_put_string_set_full(OrderedSet **s, स्थिर काष्ठा hash_ops *hash_ops, OrderedSet *l) {
        पूर्णांक r, ret = 0;
        अक्षर *p;

        निश्चित(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                अगर (r < 0)
                        वापस r;

                ret = ret || r > 0;
        }

        वापस ret;
}

व्योम ordered_set_print(खाता *f, स्थिर अक्षर *field, OrderedSet *s) {
        bool space = false;
        अक्षर *p;

        निश्चित(f);
        निश्चित(field);

        अगर (ordered_set_isempty(s))
                वापस;

        ख_माला_दो(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, शून्य, &space);

        ख_अक्षर_दो('\n', f);
}
