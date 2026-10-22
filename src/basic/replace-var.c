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

        如果 (*b != '@')
                返回 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        如果 (k <= 0 || b[k+1] != '@')
                返回 0;

        t = strndup(b + 1, k);
        如果 (!t)
                返回 -ENOMEM;

        *r = t;
        返回 1;
}

char* replace_var(const char *text, char *(*lookup)(const char *variable, void *userdata), void *userdata) {
        char *r, *t;
        const char *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(char, l+1);
        如果 (!r)
                返回 NULL;

        f = text;
        t = r;
        当 (*f) {
                _cleanup_free_ char *v = NULL, *n = NULL;
                char *a;
                整数 k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                如果 (k < 0)
                        goto oom;
                如果 (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                如果 (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                如果 (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        返回 r;

oom:
        返回 mfree(r);
}
