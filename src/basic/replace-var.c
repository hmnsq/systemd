/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "replace-var.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, ಅಕ್ಷರ **r) {
        size_t k;
        ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(b);
        ಪ್ರತಿಜ್ಞಾ(r);

        ಆದರೆ (*b != '@')
                ಹಿಂತಿರುಗಿ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ಆದರೆ (k <= 0 || b[k+1] != '@')
                ಹಿಂತಿರುಗಿ 0;

        t = strndup(b + 1, k);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *r = t;
        ಹಿಂತಿರುಗಿ 1;
}

ಅಕ್ಷರ* replace_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಅಕ್ಷರ *(*lookup)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಶೂನ್ಯ *userdata), ಶೂನ್ಯ *userdata) {
        ಅಕ್ಷರ *r, *t;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(ಪಠ್ಯ);
        ಪ್ರತಿಜ್ಞಾ(lookup);

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ);
        r = new(ಅಕ್ಷರ, l+1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        f = ಪಠ್ಯ;
        t = r;
        ಆಗ (*f) {
                _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ, *n = ಖಾಲಿ;
                ಅಕ್ಷರ *a;
                ಪೂರ್ಣಾಂಕ k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ಆದರೆ (k < 0)
                        ಹೋಗು oom;
                ಆದರೆ (k == 0) {
                        *(t++) = *(f++);
                        ಮುಂದುವರಿ;
                }

                n = lookup(v, userdata);
                ಆದರೆ (!n)
                        ಹೋಗು oom;

                skip = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(v) + 2;

                d = t - r;
                nl = l - skip + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(n);
                a = realloc(r, nl + 1);
                ಆದರೆ (!a)
                        ಹೋಗು oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ಹಿಂತಿರುಗಿ r;

oom:
        ಹಿಂತಿರುಗಿ mfree(r);
}
