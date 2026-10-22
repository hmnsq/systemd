/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "replace-var.h"
#include "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

static 정수 get_variable(const char *b, char **r) {
        size_t k;
        char *t;

        assert(b);
        assert(r);

        만약 (*b != '@')
                반환 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        만약 (k <= 0 || b[k+1] != '@')
                반환 0;

        t = strndup(b + 1, k);
        만약 (!t)
                반환 -ENOMEM;

        *r = t;
        반환 1;
}

char* replace_var(const char *text, char *(*lookup)(const char *variable, void *userdata), void *userdata) {
        char *r, *t;
        const char *f;
        size_t l;

        assert(text);
        assert(lookup);

        l = strlen(text);
        r = new(char, l+1);
        만약 (!r)
                반환 NULL;

        f = text;
        t = r;
        동안 (*f) {
                _cleanup_free_ char *v = NULL, *n = NULL;
                char *a;
                정수 k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                만약 (k < 0)
                        goto oom;
                만약 (k == 0) {
                        *(t++) = *(f++);
                        continue;
                }

                n = lookup(v, userdata);
                만약 (!n)
                        goto oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                만약 (!a)
                        goto oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        반환 r;

oom:
        반환 mfree(r);
}
