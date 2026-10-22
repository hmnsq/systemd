/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "replace-var.h"
#شامل "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ایستا صحیح get_variable(ثابت نویسه *b, نویسه **r) {
        size_t k;
        نویسه *t;

        أكد(b);
        أكد(r);

        اگر (*b != '@')
                بازگشت 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        اگر (k <= 0 || b[k+1] != '@')
                بازگشت 0;

        t = strndup(b + 1, k);
        اگر (!t)
                بازگشت -ENOMEM;

        *r = t;
        بازگشت 1;
}

نویسه* replace_var(ثابت نویسه *text, نویسه *(*lookup)(ثابت نویسه *variable, تهی *userdata), تهی *userdata) {
        نویسه *r, *t;
        ثابت نویسه *f;
        size_t l;

        أكد(text);
        أكد(lookup);

        l = strlen(text);
        r = new(نویسه, l+1);
        اگر (!r)
                بازگشت پوچ;

        f = text;
        t = r;
        وقتی (*f) {
                _cleanup_free_ نویسه *v = پوچ, *n = پوچ;
                نویسه *a;
                صحیح k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                اگر (k < 0)
                        برو oom;
                اگر (k == 0) {
                        *(t++) = *(f++);
                        ادامه;
                }

                n = lookup(v, userdata);
                اگر (!n)
                        برو oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                اگر (!a)
                        برو oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        بازگشت r;

oom:
        بازگشت mfree(r);
}
