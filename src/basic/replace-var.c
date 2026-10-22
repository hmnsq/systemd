/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "replace-var.h"
#అన్తర్భావ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

స్థిర పూర్ణాంకం get_variable(స్థిరరాశి అక్షరం *b, అక్షరం **r) {
        size_t k;
        అక్షరం *t;

        ప్రతిజ్ఞా(b);
        ప్రతిజ్ఞా(r);

        ఒకవేళ (*b != '@')
                తిరిగి 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        ఒకవేళ (k <= 0 || b[k+1] != '@')
                తిరిగి 0;

        t = strndup(b + 1, k);
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        *r = t;
        తిరిగి 1;
}

అక్షరం* replace_var(స్థిరరాశి అక్షరం *పాఠ్యం, అక్షరం *(*lookup)(స్థిరరాశి అక్షరం *variable, శూన్య *userdata), శూన్య *userdata) {
        అక్షరం *r, *t;
        స్థిరరాశి అక్షరం *f;
        size_t l;

        ప్రతిజ్ఞా(పాఠ్యం);
        ప్రతిజ్ఞా(lookup);

        l = అక్షరశ్రేణి_పొడవు(పాఠ్యం);
        r = new(అక్షరం, l+1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        f = పాఠ్యం;
        t = r;
        అయితే (*f) {
                _cleanup_free_ అక్షరం *v = ఖాళీ, *n = ఖాళీ;
                అక్షరం *a;
                పూర్ణాంకం k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                ఒకవేళ (k < 0)
                        వెళ్ళు oom;
                ఒకవేళ (k == 0) {
                        *(t++) = *(f++);
                        కొనసాగు;
                }

                n = lookup(v, userdata);
                ఒకవేళ (!n)
                        వెళ్ళు oom;

                skip = అక్షరశ్రేణి_పొడవు(v) + 2;

                d = t - r;
                nl = l - skip + అక్షరశ్రేణి_పొడవు(n);
                a = realloc(r, nl + 1);
                ఒకవేళ (!a)
                        వెళ్ళు oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        తిరిగి r;

oom:
        తిరిగి mfree(r);
}
