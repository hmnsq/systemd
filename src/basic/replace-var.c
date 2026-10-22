/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static số_nguyên get_variable(const char *b, char **r) {
        size_t k;
        char *t;

        assert(b);
        assert(r);

        nếu (*b != '@')
                trả_về 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        nếu (k <= 0 || b[k+1] != '@')
                trả_về 0;

        t = strndup(b + 1, k);
        nếu (!t)
                trả_về -ENOMEM;

        *r = t;
        trả_về 1;
}

char* replace_var(const char *text, char *(*lookup)(const char *variable, void *userdata), void *userdata) {
        char *r, *t;
        const char *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(char, l+1);
        nếu (!r)
                trả_về NULL;

        f = text;
        t = r;
        trong_khi (*f) {
                _cleanup_free_ char *v = NULL, *n = NULL;
                char *a;
                số_nguyên k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                nếu (k < 0)
                        goto oom;
                nếu (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                nếu (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                nếu (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        trả_về r;

oom:
        trả_về mfree(r);
}
