/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "replace-var.h"
#شامل "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ساکن صحیح get_variable(ثابت حرف *b, حرف **r) {
        size_t k;
        حرف *t;

        أكد(b);
        أكد(r);

        اگر (*b != '@')
                واپس 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        اگر (k <= 0 || b[k+1] != '@')
                واپس 0;

        t = strndup(b + 1, k);
        اگر (!t)
                واپس -ENOMEM;

        *r = t;
        واپس 1;
}

حرف* replace_var(ثابت حرف *متن, حرف *(*lookup)(ثابت حرف *variable, خالی *userdata), خالی *userdata) {
        حرف *r, *t;
        ثابت حرف *f;
        size_t l;

        أكد(متن);
        أكد(lookup);

        l = لڑی_لمبائی(متن);
        r = new(حرف, l+1);
        اگر (!r)
                واپس تہی;

        f = متن;
        t = r;
        جب (*f) {
                _cleanup_free_ حرف *v = تہی, *n = تہی;
                حرف *a;
                صحیح k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                اگر (k < 0)
                        جاؤ oom;
                اگر (k == 0) {
                        *(t++) = *(f++);
                        جاری;
                }

                n = lookup(v, userdata);
                اگر (!n)
                        جاؤ oom;

                skip = لڑی_لمبائی(v) + 2;

                d = t - r;
                nl = l - skip + لڑی_لمبائی(n);
                a = realloc(r, nl + 1);
                اگر (!a)
                        جاؤ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        واپس r;

oom:
        واپس mfree(r);
}
