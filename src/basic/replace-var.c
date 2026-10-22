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

        إذا (*b != '@')
                أرجع 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        إذا (k <= 0 || b[k+1] != '@')
                أرجع 0;

        t = strndup(b + 1, k);
        إذا (!t)
                أرجع -ENOMEM;

        *r = t;
        أرجع 1;
}

حرف* replace_var(ثابت حرف *متن, حرف *(*lookup)(ثابت حرف *variable, فراغ *userdata), فراغ *userdata) {
        حرف *r, *t;
        ثابت حرف *f;
        size_t l;

        أكد(متن);
        أكد(lookup);

        l = لڙي_ڊيگهه(متن);
        r = new(حرف, l+1);
        إذا (!r)
                أرجع عدم;

        f = متن;
        t = r;
        طالما (*f) {
                _cleanup_free_ حرف *v = عدم, *n = عدم;
                حرف *a;
                صحيح k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                إذا (k < 0)
                        اذهب oom;
                إذا (k == 0) {
                        *(t++) = *(f++);
                        استمر;
                }

                n = lookup(v, userdata);
                إذا (!n)
                        اذهب oom;

                skip = لڙي_ڊيگهه(v) + 2;

                d = t - r;
                nl = l - skip + لڙي_ڊيگهه(n);
                a = realloc(r, nl + 1);
                إذا (!a)
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
