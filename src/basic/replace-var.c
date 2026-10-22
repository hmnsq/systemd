/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "replace-var.h"
#অন্তর্ভাৱ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

স্থির পূর্ণাঙ্ক get_variable(ধ্রুৱ ৱর্ণ *b, ৱর্ণ **r) {
        size_t k;
        ৱর্ণ *t;

        প্রতিজ্ঞা(b);
        প্রতিজ্ঞা(r);

        যদি (*b != '@')
                ঘূৰাই 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        যদি (k <= 0 || b[k+1] != '@')
                ঘূৰাই 0;

        t = strndup(b + 1, k);
        যদি (!t)
                ঘূৰাই -ENOMEM;

        *r = t;
        ঘূৰাই 1;
}

ৱর্ণ* replace_var(ধ্রুৱ ৱর্ণ *text, ৱর্ণ *(*lookup)(ধ্রুৱ ৱর্ণ *variable, শূন্য *userdata), শূন্য *userdata) {
        ৱর্ণ *r, *t;
        ধ্রুৱ ৱর্ণ *f;
        size_t l;

        প্রতিজ্ঞা(text);
        প্রতিজ্ঞা(lookup);

        l = strlen(text);
        r = new(ৱর্ণ, l+1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        f = text;
        t = r;
        যেতিয়ালৈ (*f) {
                _cleanup_free_ ৱর্ণ *v = রিক্ত, *n = রিক্ত;
                ৱর্ণ *a;
                পূর্ণাঙ্ক k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                যদি (k < 0)
                        গচ্ছ oom;
                যদি (k == 0) {
                        *(t++) = *(f++);
                        অনুৱর্ত;
                }

                n = lookup(v, userdata);
                যদি (!n)
                        গচ্ছ oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                যদি (!a)
                        গচ্ছ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ঘূৰাই r;

oom:
        ঘূৰাই mfree(r);
}
