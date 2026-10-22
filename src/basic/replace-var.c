/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "replace-var.h"
#அந்தர்பாவ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

நிலையான முழு get_variable(மாறிலி எழுத்து *b, எழுத்து **r) {
        size_t k;
        எழுத்து *t;

        ப்ரதிஜ்ஞா(b);
        ப்ரதிஜ்ஞா(r);

        எனில் (*b != '@')
                திரும்பு 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        எனில் (k <= 0 || b[k+1] != '@')
                திரும்பு 0;

        t = strndup(b + 1, k);
        எனில் (!t)
                திரும்பு -ENOMEM;

        *r = t;
        திரும்பு 1;
}

எழுத்து* replace_var(மாறிலி எழுத்து *text, எழுத்து *(*lookup)(மாறிலி எழுத்து *variable, வெறுமை *userdata), வெறுமை *userdata) {
        எழுத்து *r, *t;
        மாறிலி எழுத்து *f;
        size_t l;

        ப்ரதிஜ்ஞா(text);
        ப்ரதிஜ்ஞா(lookup);

        l = strlen(text);
        r = new(எழுத்து, l+1);
        எனில் (!r)
                திரும்பு வெற்று;

        f = text;
        t = r;
        போது (*f) {
                _cleanup_free_ எழுத்து *v = வெற்று, *n = வெற்று;
                எழுத்து *a;
                முழு k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                எனில் (k < 0)
                        செல் oom;
                எனில் (k == 0) {
                        *(t++) = *(f++);
                        தொடர்;
                }

                n = lookup(v, userdata);
                எனில் (!n)
                        செல் oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                எனில் (!a)
                        செல் oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        திரும்பு r;

oom:
        திரும்பு mfree(r);
}
