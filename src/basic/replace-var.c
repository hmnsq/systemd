/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static ཧྲིལ get_variable(const var_na *b, var_na **r) {
        size_t k;
        var_na *t;

        assert(b);
        assert(r);

        གལ་ཏེ (*b != '@')
                ཕྱིར 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        གལ་ཏེ (k <= 0 || b[k+1] != '@')
                ཕྱིར 0;

        t = strndup(b + 1, k);
        གལ་ཏེ (!t)
                ཕྱིར -ENOMEM;

        *r = t;
        ཕྱིར 1;
}

var_na* replace_var(const var_na *text, var_na *(*lookup)(const var_na *variable, void *userdata), void *userdata) {
        var_na *r, *t;
        const var_na *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(var_na, l+1);
        གལ་ཏེ (!r)
                ཕྱིར NULL;

        f = text;
        t = r;
        སྐབས (*f) {
                _cleanup_free_ var_na *v = NULL, *n = NULL;
                var_na *a;
                ཧྲིལ k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                གལ་ཏེ (k < 0)
                        goto oom;
                གལ་ཏེ (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                གལ་ཏེ (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                གལ་ཏེ (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ཕྱིར r;

oom:
        ཕྱིར mfree(r);
}
