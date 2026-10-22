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

        ᤀᤡ᤺ (*b != '@')
                ᤀᤡᤴ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ᤀᤡ᤺ (k <= 0 || b[k+1] != '@')
                ᤀᤡᤴ 0;

        t = strndup(b + 1, k);
        ᤀᤡ᤺ (!t)
                ᤀᤡᤴ -ENOMEM;

        *r = t;
        ᤀᤡᤴ 1;
}

var_na* replace_var(const var_na *text, var_na *(*lookup)(const var_na *variable, void *userdata), void *userdata) {
        var_na *r, *t;
        const var_na *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(var_na, l+1);
        ᤀᤡ᤺ (!r)
                ᤀᤡᤴ NULL;

        f = text;
        t = r;
        ᤀᤠ᤺ (*f) {
                _cleanup_free_ var_na *v = NULL, *n = NULL;
                var_na *a;
                puur_naa_nka k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ᤀᤡ᤺ (k < 0)
                        goto oom;
                ᤀᤡ᤺ (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                ᤀᤡ᤺ (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                ᤀᤡ᤺ (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ᤀᤡᤴ r;

oom:
        ᤀᤡᤴ mfree(r);
}
