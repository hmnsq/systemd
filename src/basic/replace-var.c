/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "replace-var.h"
#अन्तर्भाव "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

स्थिर पूर्णाङ्क get_variable(ध्रुव वर्ण *b, वर्ण **r) {
        size_t k;
        वर्ण *t;

        प्रतिज्ञा(b);
        प्रतिज्ञा(r);

        जइ (*b != '@')
                प्रत्यागम 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        जइ (k <= 0 || b[k+1] != '@')
                प्रत्यागम 0;

        t = strndup(b + 1, k);
        जइ (!t)
                प्रत्यागम -ENOMEM;

        *r = t;
        प्रत्यागम 1;
}

वर्ण* replace_var(ध्रुव वर्ण *text, वर्ण *(*lookup)(ध्रुव वर्ण *variable, शून्य *userdata), शून्य *userdata) {
        वर्ण *r, *t;
        ध्रुव वर्ण *f;
        size_t l;

        प्रतिज्ञा(text);
        प्रतिज्ञा(lookup);

        l = strlen(text);
        r = new(वर्ण, l+1);
        जइ (!r)
                प्रत्यागम रिक्त;

        f = text;
        t = r;
        जाव (*f) {
                _cleanup_free_ वर्ण *v = रिक्त, *n = रिक्त;
                वर्ण *a;
                पूर्णाङ्क k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                जइ (k < 0)
                        गच्छ oom;
                जइ (k == 0) {
                        *(t++) = *(f++);
                        अनुवर्त;
                }

                n = lookup(v, userdata);
                जइ (!n)
                        गच्छ oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                जइ (!a)
                        गच्छ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        प्रत्यागम r;

oom:
        प्रत्यागम mfree(r);
}
