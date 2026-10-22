/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static ကိန်းပြည့် get_variable(const var_na *b, var_na **r) {
        size_t k;
        var_na *t;

        assert(b);
        assert(r);

        အကယ်၍ (*b != '@')
                ပြန်ပေး 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        အကယ်၍ (k <= 0 || b[k+1] != '@')
                ပြန်ပေး 0;

        t = strndup(b + 1, k);
        အကယ်၍ (!t)
                ပြန်ပေး -ENOMEM;

        *r = t;
        ပြန်ပေး 1;
}

var_na* replace_var(const var_na *text, var_na *(*lookup)(const var_na *variable, void *userdata), void *userdata) {
        var_na *r, *t;
        const var_na *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(var_na, l+1);
        အကယ်၍ (!r)
                ပြန်ပေး NULL;

        f = text;
        t = r;
        နေစဉ် (*f) {
                _cleanup_free_ var_na *v = NULL, *n = NULL;
                var_na *a;
                ကိန်းပြည့် k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                အကယ်၍ (k < 0)
                        goto oom;
                အကယ်၍ (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                အကယ်၍ (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                အကယ်၍ (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ပြန်ပေး r;

oom:
        ပြန်ပေး mfree(r);
}
