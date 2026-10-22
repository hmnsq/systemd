/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "replace-var.h"
#अन्तर्भाव "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

स्थिर पूर्णांक get_variable(स्थिरांक अक्षर *b, अक्षर **r) {
        माप_प्रकार k;
        अक्षर *t;

        प्रतिज्ञा(b);
        प्रतिज्ञा(r);

        जर (*b != '@')
                परत 0;

        k = म_अखोज(b + 1, UPPERCASE_LETTERS "_");
        जर (k <= 0 || b[k+1] != '@')
                परत 0;

        t = strndup(b + 1, k);
        जर (!t)
                परत -ENOMEM;

        *r = t;
        परत 1;
}

अक्षर* replace_var(स्थिरांक अक्षर *text, अक्षर *(*lookup)(स्थिरांक अक्षर *variable, शून्य *userdata), शून्य *userdata) {
        अक्षर *r, *t;
        स्थिरांक अक्षर *f;
        माप_प्रकार l;

        प्रतिज्ञा(text);
        प्रतिज्ञा(lookup);

        l = म_माप(text);
        r = new(अक्षर, l+1);
        जर (!r)
                परत रिक्त;

        f = text;
        t = r;
        पर्यंत (*f) {
                _cleanup_free_ अक्षर *v = रिक्त, *n = रिक्त;
                अक्षर *a;
                पूर्णांक k;
                माप_प्रकार skip, d, nl;

                k = get_variable(f, &v);
                जर (k < 0)
                        जा oom;
                जर (k == 0) {
                        *(t++) = *(f++);
                        सुरूठेवा;
                }

                n = lookup(v, userdata);
                जर (!n)
                        जा oom;

                skip = म_माप(v) + 2;

                d = t - r;
                nl = l - skip + म_माप(n);
                a = पुनः_स्मृति(r, nl + 1);
                जर (!a)
                        जा oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        परत r;

oom:
        परत mfree(r);
}
