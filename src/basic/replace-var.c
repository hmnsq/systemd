/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "replace-var.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ਸਥਿਰ ਪੂਰਨ get_variable(ਅਚਲ ਅੱਖਰ *b, ਅੱਖਰ **r) {
        size_t k;
        ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(b);
        ਪ੍ਰਤਿਜ੍ਞਾ(r);

        ਜੇ (*b != '@')
                ਵਾਪਸ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ਜੇ (k <= 0 || b[k+1] != '@')
                ਵਾਪਸ 0;

        t = strndup(b + 1, k);
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        *r = t;
        ਵਾਪਸ 1;
}

ਅੱਖਰ* replace_var(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਅੱਖਰ *(*lookup)(ਅਚਲ ਅੱਖਰ *variable, ਖਾਲੀ *userdata), ਖਾਲੀ *userdata) {
        ਅੱਖਰ *r, *t;
        ਅਚਲ ਅੱਖਰ *f;
        size_t l;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਪਾਠ);
        ਪ੍ਰਤਿਜ੍ਞਾ(lookup);

        l = ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ);
        r = new(ਅੱਖਰ, l+1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        f = ਪਾਠ;
        t = r;
        ਜਦੋਂ (*f) {
                _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ, *n = ਨਿਰਾ;
                ਅੱਖਰ *a;
                ਪੂਰਨ k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ਜੇ (k < 0)
                        ਜਾਓ oom;
                ਜੇ (k == 0) {
                        *(t++) = *(f++);
                        ਜਾਰੀ;
                }

                n = lookup(v, userdata);
                ਜੇ (!n)
                        ਜਾਓ oom;

                skip = ਲੜੀ_ਲੰਬਾਈ(v) + 2;

                d = t - r;
                nl = l - skip + ਲੜੀ_ਲੰਬਾਈ(n);
                a = realloc(r, nl + 1);
                ਜੇ (!a)
                        ਜਾਓ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ਵਾਪਸ r;

oom:
        ਵਾਪਸ mfree(r);
}
