/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static 整数 get_variable(const char *b, char **r) {
        size_t k;
        char *t;

        assert(b);
        assert(r);

        もし (*b != '@')
                戻る 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        もし (k <= 0 || b[k+1] != '@')
                戻る 0;

        t = strndup(b + 1, k);
        もし (!t)
                戻る -ENOMEM;

        *r = t;
        戻る 1;
}

char* replace_var(const char *text, char *(*lookup)(const char *variable, void *userdata), void *userdata) {
        char *r, *t;
        const char *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(char, l+1);
        もし (!r)
                戻る NULL;

        f = text;
        t = r;
        ながら (*f) {
                _cleanup_free_ char *v = NULL, *n = NULL;
                char *a;
                整数 k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                もし (k < 0)
                        goto oom;
                もし (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                もし (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                もし (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        戻る r;

oom:
        戻る mfree(r);
}
