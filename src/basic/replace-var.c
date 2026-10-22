/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "replace-var.h"
#ضمن "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ساكن صحيح get_variable(ثابت حرف *b, حرف **r) {
        size_t k;
        حرف *t;

        أكد(b);
        أكد(r);

        𐡄𐡍 (*b != '@')
                أرجع 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        𐡄𐡍 (k <= 0 || b[k+1] != '@')
                أرجع 0;

        t = strndup(b + 1, k);
        𐡄𐡍 (!t)
                أرجع -ENOMEM;

        *r = t;
        أرجع 1;
}

حرف* replace_var(ثابت حرف *text, حرف *(*lookup)(ثابت حرف *variable, فراغ *userdata), فراغ *userdata) {
        حرف *r, *t;
        ثابت حرف *f;
        size_t l;

        أكد(text);
        أكد(lookup);

        l = strlen(text);
        r = new(حرف, l+1);
        𐡄𐡍 (!r)
                أرجع عدم;

        f = text;
        t = r;
        طالما (*f) {
                _cleanup_free_ حرف *v = عدم, *n = عدم;
                حرف *a;
                صحيح k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                𐡄𐡍 (k < 0)
                        اذهب oom;
                𐡄𐡍 (k == 0) {
                        *(t++) = *(f++);
                        استمر;
                }

                n = lookup(v, userdata);
                𐡄𐡍 (!n)
                        اذهب oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                𐡄𐡍 (!a)
                        اذهب oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        أرجع r;

oom:
        أرجع mfree(r);
}
