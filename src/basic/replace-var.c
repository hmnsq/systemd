/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "replace-var.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_variable(ଅଚଳ ଅକ୍ଷର *b, ଅକ୍ଷର **r) {
        size_t k;
        ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(b);
        ପ୍ରତିଜ୍ଞା(r);

        ଯଦି (*b != '@')
                ଫେରନ୍ତୁ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ଯଦି (k <= 0 || b[k+1] != '@')
                ଫେରନ୍ତୁ 0;

        t = strndup(b + 1, k);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        *r = t;
        ଫେରନ୍ତୁ 1;
}

ଅକ୍ଷର* replace_var(ଅଚଳ ଅକ୍ଷର *text, ଅକ୍ଷର *(*lookup)(ଅଚଳ ଅକ୍ଷର *variable, ଶୂନ୍ୟ *userdata), ଶୂନ୍ୟ *userdata) {
        ଅକ୍ଷର *r, *t;
        ଅଚଳ ଅକ୍ଷର *f;
        size_t l;

        ପ୍ରତିଜ୍ଞା(text);
        ପ୍ରତିଜ୍ଞା(lookup);

        l = strlen(text);
        r = new(ଅକ୍ଷର, l+1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        f = text;
        t = r;
        ଯେତେବେଳେ (*f) {
                _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି, *n = ଖାଲି;
                ଅକ୍ଷର *a;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ଯଦି (k < 0)
                        ଯାଅ oom;
                ଯଦି (k == 0) {
                        *(t++) = *(f++);
                        ଜାରି;
                }

                n = lookup(v, userdata);
                ଯଦି (!n)
                        ଯାଅ oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                ଯଦି (!a)
                        ଯାଅ oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        ଫେରନ୍ତୁ r;

oom:
        ଫେରନ୍ତୁ mfree(r);
}
