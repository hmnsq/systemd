/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "replace-var.h"
#अन्तर्भाव "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

स्थिर पूर्णाङ्क get_variable(अचर वर्ण *b, वर्ण **r) {
        माप_प्रकार k;
        वर्ण *t;

        प्रतिज्ञा(b);
        प्रतिज्ञा(r);

        यदि (*b != '@')
                फर्काऊ 0;

        k = म_अखोज(b + 1, UPPERCASE_LETTERS "_");
        यदि (k <= 0 || b[k+1] != '@')
                फर्काऊ 0;

        t = strndup(b + 1, k);
        यदि (!t)
                फर्काऊ -ENOMEM;

        *r = t;
        फर्काऊ 1;
}

वर्ण* replace_var(अचर वर्ण *text, वर्ण *(*lookup)(अचर वर्ण *variable, शून्य *userdata), शून्य *userdata) {
        वर्ण *r, *t;
        अचर वर्ण *f;
        माप_प्रकार l;

        प्रतिज्ञा(text);
        प्रतिज्ञा(lookup);

        l = म_माप(text);
        r = new(वर्ण, l+1);
        यदि (!r)
                फर्काऊ रिक्त;

        f = text;
        t = r;
        जबसम्म (*f) {
                _cleanup_free_ वर्ण *v = रिक्त, *n = रिक्त;
                वर्ण *a;
                पूर्णाङ्क k;
                माप_प्रकार skip, d, nl;

                k = get_variable(f, &v);
                यदि (k < 0)
                        गच्छ oom;
                यदि (k == 0) {
                        *(t++) = *(f++);
                        जारी;
                }

                n = lookup(v, userdata);
                यदि (!n)
                        गच्छ oom;

                skip = म_माप(v) + 2;

                d = t - r;
                nl = l - skip + म_माप(n);
                a = पुनः_स्मृति(r, nl + 1);
                यदि (!a)
                        गच्छ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        फर्काऊ r;

oom:
        फर्काऊ mfree(r);
}
