/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "replace-var.h"
#समावेश "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

अटल पूर्णांक get_variable(स्थिर अक्षर *b, अक्षर **r) {
        माप_प्रकार k;
        अक्षर *t;

        निश्चित(b);
        निश्चित(r);

        अगर (*b != '@')
                वापस 0;

        k = म_अखोज(b + 1, UPPERCASE_LETTERS "_");
        अगर (k <= 0 || b[k+1] != '@')
                वापस 0;

        t = strndup(b + 1, k);
        अगर (!t)
                वापस -ENOMEM;

        *r = t;
        वापस 1;
}

अक्षर* replace_var(स्थिर अक्षर *text, अक्षर *(*lookup)(स्थिर अक्षर *variable, व्योम *userdata), व्योम *userdata) {
        अक्षर *r, *t;
        स्थिर अक्षर *f;
        माप_प्रकार l;

        निश्चित(text);
        निश्चित(lookup);

        l = म_माप(text);
        r = new(अक्षर, l+1);
        अगर (!r)
                वापस शून्य;

        f = text;
        t = r;
        जबतक (*f) {
                _cleanup_free_ अक्षर *v = शून्य, *n = शून्य;
                अक्षर *a;
                पूर्णांक k;
                माप_प्रकार skip, d, nl;

                k = get_variable(f, &v);
                अगर (k < 0)
                        जाओ oom;
                अगर (k == 0) {
                        *(t++) = *(f++);
                        जारी;
                }

                n = lookup(v, userdata);
                अगर (!n)
                        जाओ oom;

                skip = म_माप(v) + 2;

                d = t - r;
                nl = l - skip + म_माप(n);
                a = पुनः_स्मृति(r, nl + 1);
                अगर (!a)
                        जाओ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        वापस r;

oom:
        वापस mfree(r);
}
