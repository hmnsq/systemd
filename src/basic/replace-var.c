/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "replace-var.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_variable(ಧ್ರುವ ವರ್ಣ *b, ವರ್ಣ **r) {
        size_t k;
        ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(b);
        ಪ್ರತಿಜ್ಞಾ(r);

        ಯದಿ (*b != '@')
                ಪ್ರತ್ಯಾಗಮ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ಯದಿ (k <= 0 || b[k+1] != '@')
                ಪ್ರತ್ಯಾಗಮ 0;

        t = strndup(b + 1, k);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *r = t;
        ಪ್ರತ್ಯಾಗಮ 1;
}

ವರ್ಣ* replace_var(ಧ್ರುವ ವರ್ಣ *text, ವರ್ಣ *(*lookup)(ಧ್ರುವ ವರ್ಣ *variable, ಶೂನ್ಯ *userdata), ಶೂನ್ಯ *userdata) {
        ವರ್ಣ *r, *t;
        ಧ್ರುವ ವರ್ಣ *f;
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(text);
        ಪ್ರತಿಜ್ಞಾ(lookup);

        l = strlen(text);
        r = new(ವರ್ಣ, l+1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        f = text;
        t = r;
        ಯಾವತ್ (*f) {
                _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ, *n = ರಿಕ್ತ;
                ವರ್ಣ *a;
                ಪೂರ್ಣಾಙ್ಕ k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ಯದಿ (k < 0)
                        ಗಚ್ಛ oom;
                ಯದಿ (k == 0) {
                        *(t++) = *(f++);
                        ಅನುವರ್ತ;
                }

                n = lookup(v, userdata);
                ಯದಿ (!n)
                        ಗಚ್ಛ oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                ಯದಿ (!a)
                        ಗಚ್ಛ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ಪ್ರತ್ಯಾಗಮ r;

oom:
        ಪ್ರತ್ಯಾಗಮ mfree(r);
}
