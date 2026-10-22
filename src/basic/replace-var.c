/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "replace-var.h"
#অন্তর্ভাব "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

স্থির পূর্ণাঙ্ক get_variable(ধ্রুব বর্ণ *b, বর্ণ **r) {
        size_t k;
        বর্ণ *t;

        প্রতিজ্ঞা(b);
        প্রতিজ্ঞা(r);

        যদি (*b != '@')
                প্রত্যাগম 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        যদি (k <= 0 || b[k+1] != '@')
                প্রত্যাগম 0;

        t = strndup(b + 1, k);
        যদি (!t)
                প্রত্যাগম -ENOMEM;

        *r = t;
        প্রত্যাগম 1;
}

বর্ণ* replace_var(ধ্রুব বর্ণ *text, বর্ণ *(*lookup)(ধ্রুব বর্ণ *variable, শূন্য *userdata), শূন্য *userdata) {
        বর্ণ *r, *t;
        ধ্রুব বর্ণ *f;
        size_t l;

        প্রতিজ্ঞা(text);
        প্রতিজ্ঞা(lookup);

        l = strlen(text);
        r = new(বর্ণ, l+1);
        যদি (!r)
                প্রত্যাগম রিক্ত;

        f = text;
        t = r;
        যাবত্ (*f) {
                _cleanup_free_ বর্ণ *v = রিক্ত, *n = রিক্ত;
                বর্ণ *a;
                পূর্ণাঙ্ক k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                যদি (k < 0)
                        গচ্ছ oom;
                যদি (k == 0) {
                        *(t++) = *(f++);
                        অনুবর্ত;
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
        প্রত্যাগম r;

oom:
        প্রত্যাগম mfree(r);
}
