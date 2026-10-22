/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static целое get_variable(const char *b, char **r) {
        size_t k;
        char *t;

        assert(b);
        assert(r);

        если (*b != '@')
                вернуть 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        если (k <= 0 || b[k+1] != '@')
                вернуть 0;

        t = strndup(b + 1, k);
        если (!t)
                вернуть -ENOMEM;

        *r = t;
        вернуть 1;
}

char* replace_var(const char *text, char *(*lookup)(const char *variable, void *userdata), void *userdata) {
        char *r, *t;
        const char *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(char, l+1);
        если (!r)
                вернуть NULL;

        f = text;
        t = r;
        пока (*f) {
                _cleanup_free_ char *v = NULL, *n = NULL;
                char *a;
                целое k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                если (k < 0)
                        goto oom;
                если (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                если (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                если (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        вернуть r;

oom:
        вернуть mfree(r);
}
