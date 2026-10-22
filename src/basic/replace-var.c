/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "replace-var.h"
#અન્તર્ભાવ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

સ્થિર પૂર્ણાંક get_variable(અચલ અક્ષર *b, અક્ષર **r) {
        size_t k;
        અક્ષર *t;

        પ્રતિજ્ઞા(b);
        પ્રતિજ્ઞા(r);

        જો (*b != '@')
                પાછા 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        જો (k <= 0 || b[k+1] != '@')
                પાછા 0;

        t = strndup(b + 1, k);
        જો (!t)
                પાછા -ENOMEM;

        *r = t;
        પાછા 1;
}

અક્ષર* replace_var(અચલ અક્ષર *text, અક્ષર *(*lookup)(અચલ અક્ષર *variable, શૂન્ય *userdata), શૂન્ય *userdata) {
        અક્ષર *r, *t;
        અચલ અક્ષર *f;
        size_t l;

        પ્રતિજ્ઞા(text);
        પ્રતિજ્ઞા(lookup);

        l = strlen(text);
        r = new(અક્ષર, l+1);
        જો (!r)
                પાછા ખાલી;

        f = text;
        t = r;
        જ્યારે (*f) {
                _cleanup_free_ અક્ષર *v = ખાલી, *n = ખાલી;
                અક્ષર *a;
                પૂર્ણાંક k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                જો (k < 0)
                        જાઓ oom;
                જો (k == 0) {
                        *(t++) = *(f++);
                        ચાલુ;
                }

                n = lookup(v, userdata);
                જો (!n)
                        જાઓ oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                જો (!a)
                        જાઓ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        પાછા r;

oom:
        પાછા mfree(r);
}
