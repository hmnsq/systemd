/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static puur_naa_nka get_variable(const var_na *b, var_na **r) {
        size_t k;
        var_na *t;

        assert(b);
        assert(r);

        ᱡᱩᱫᱤ (*b != '@')
                ᱨᱩᱣᱟᱹᱲ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ᱡᱩᱫᱤ (k <= 0 || b[k+1] != '@')
                ᱨᱩᱣᱟᱹᱲ 0;

        t = strndup(b + 1, k);
        ᱡᱩᱫᱤ (!t)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        *r = t;
        ᱨᱩᱣᱟᱹᱲ 1;
}

var_na* replace_var(const var_na *text, var_na *(*lookup)(const var_na *variable, void *userdata), void *userdata) {
        var_na *r, *t;
        const var_na *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(var_na, l+1);
        ᱡᱩᱫᱤ (!r)
                ᱨᱩᱣᱟᱹᱲ NULL;

        f = text;
        t = r;
        ᱡᱚᱠᱷᱚᱱ (*f) {
                _cleanup_free_ var_na *v = NULL, *n = NULL;
                var_na *a;
                puur_naa_nka k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ᱡᱩᱫᱤ (k < 0)
                        goto oom;
                ᱡᱩᱫᱤ (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                ᱡᱩᱫᱤ (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                ᱡᱩᱫᱤ (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ᱨᱩᱣᱟᱹᱲ r;

oom:
        ᱨᱩᱣᱟᱹᱲ mfree(r);
}
