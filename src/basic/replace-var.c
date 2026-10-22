/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "replace-var.h"
#অন্তর্ভাব "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

স্থির পূর্ণসংখ্যা get_variable(ধ্রুবক অক্ষর *b, অক্ষর **r) {
        size_t k;
        অক্ষর *t;

        প্রতিজ্ঞা(b);
        প্রতিজ্ঞা(r);

        যদি (*b != '@')
                ফেরত 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        যদি (k <= 0 || b[k+1] != '@')
                ফেরত 0;

        t = strndup(b + 1, k);
        যদি (!t)
                ফেরত -ENOMEM;

        *r = t;
        ফেরত 1;
}

অক্ষর* replace_var(ধ্রুবক অক্ষর *text, অক্ষর *(*lookup)(ধ্রুবক অক্ষর *variable, শূন্য *userdata), শূন্য *userdata) {
        অক্ষর *r, *t;
        ধ্রুবক অক্ষর *f;
        size_t l;

        প্রতিজ্ঞা(text);
        প্রতিজ্ঞা(lookup);

        l = strlen(text);
        r = new(অক্ষর, l+1);
        যদি (!r)
                ফেরত ফাঁকা;

        f = text;
        t = r;
        যখন (*f) {
                _cleanup_free_ অক্ষর *v = ফাঁকা, *n = ফাঁকা;
                অক্ষর *a;
                পূর্ণসংখ্যা k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                যদি (k < 0)
                        যাও oom;
                যদি (k == 0) {
                        *(t++) = *(f++);
                        চালু;
                }

                n = lookup(v, userdata);
                যদি (!n)
                        যাও oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                যদি (!a)
                        যাও oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ফেরত r;

oom:
        ফেরত mfree(r);
}
