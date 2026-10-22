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

        אם (*b != '@')
                החזר 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        אם (k <= 0 || b[k+1] != '@')
                החזר 0;

        t = strndup(b + 1, k);
        אם (!t)
                החזר -ENOMEM;

        *r = t;
        החזר 1;
}

حرف* replace_var(ثابت حرف *text, حرف *(*lookup)(ثابت حرف *variable, فراغ *userdata), فراغ *userdata) {
        حرف *r, *t;
        ثابت حرف *f;
        size_t l;

        أكد(text);
        أكد(lookup);

        l = strlen(text);
        r = new(حرف, l+1);
        אם (!r)
                החזר عدم;

        f = text;
        t = r;
        כלעוד (*f) {
                _cleanup_free_ حرف *v = عدم, *n = عدم;
                حرف *a;
                صحيح k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                אם (k < 0)
                        اذهب oom;
                אם (k == 0) {
                        *(t++) = *(f++);
                        استمر;
                }

                n = lookup(v, userdata);
                אם (!n)
                        اذهب oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                אם (!a)
                        اذهب oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        החזר r;

oom:
        החזר mfree(r);
}
