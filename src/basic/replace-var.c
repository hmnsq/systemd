/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static ចំនួនគត់ get_variable(const var_na *b, var_na **r) {
        size_t k;
        var_na *t;

        assert(b);
        assert(r);

        ប្រសិនបើ (*b != '@')
                ត្រឡប់ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ប្រសិនបើ (k <= 0 || b[k+1] != '@')
                ត្រឡប់ 0;

        t = strndup(b + 1, k);
        ប្រសិនបើ (!t)
                ត្រឡប់ -ENOMEM;

        *r = t;
        ត្រឡប់ 1;
}

var_na* replace_var(const var_na *text, var_na *(*lookup)(const var_na *variable, void *userdata), void *userdata) {
        var_na *r, *t;
        const var_na *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(var_na, l+1);
        ប្រសិនបើ (!r)
                ត្រឡប់ NULL;

        f = text;
        t = r;
        ខណៈពេល (*f) {
                _cleanup_free_ var_na *v = NULL, *n = NULL;
                var_na *a;
                ចំនួនគត់ k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ប្រសិនបើ (k < 0)
                        goto oom;
                ប្រសិនបើ (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                ប្រសិនបើ (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                ប្រសិនបើ (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ត្រឡប់ r;

oom:
        ត្រឡប់ mfree(r);
}
