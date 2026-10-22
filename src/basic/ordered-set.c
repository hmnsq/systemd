/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "fileio.h"
#అన్తర్భావ "ordered-set.h"
#అన్తర్భావ "strv.h"

పూర్ణాంకం ordered_సమితి_ensure_allocated(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *ops) {
        ప్రతిజ్ఞా(s);

        ఒకవేళ (*s)
                తిరిగి 0;

        *s = ordered_సమితి_new(ops);
        ఒకవేళ (!*s)
                తిరిగి -ENOMEM;

        తిరిగి 0;
}

పూర్ణాంకం ordered_సమితి_ensure_put(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *ops, శూన్య *p) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        r = ordered_సమితి_ensure_allocated(s, ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ordered_సమితి_put(*s, p);
}

పూర్ణాంకం ordered_సమితి_consume(Ordered_సమితి *s, శూన్య *p) {
        పూర్ణాంకం r;

        r = ordered_సమితి_put(s, p);
        ఒకవేళ (r <= 0)
                విడుదల(p);

        తిరిగి r;
}

పూర్ణాంకం ordered_సమితి_put_strdup_నిండిన(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి అక్షరం *p) {
        అక్షరం *c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(p);

        r = ordered_సమితి_ensure_allocated(s, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ordered_సమితి_కలిగియుంది(*s, p))
                తిరిగి 0;

        c = strdup(p);
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        తిరిగి ordered_సమితి_consume(*s, c);
}

పూర్ణాంకం ordered_సమితి_put_strdupv_నిండిన(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, అక్షరం **l) {
        పూర్ణాంకం r, ret = 0;

        ప్రతిజ్ఞా(s);

        STRV_FOREACH(i, l) {
                r = ordered_సమితి_put_strdup_నిండిన(s, hash_ops, *i);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ret = ret || r > 0;
        }

        తిరిగి ret;
}

పూర్ణాంకం ordered_సమితి_put_string_సమితి_నిండిన(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, Ordered_సమితి *l) {
        పూర్ణాంకం r, ret = 0;
        అక్షరం *p;

        ప్రతిజ్ఞా(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_సమితి_FOREACH(p, l) {
                r = ordered_సమితి_put_strdup_నిండిన(s, hash_ops, p);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ret = ret || r > 0;
        }

        తిరిగి ret;
}

శూన్య ordered_సమితి_print(FILE *f, స్థిరరాశి అక్షరం *క్షేత్రం, Ordered_సమితి *s) {
        bool space = false;
        అక్షరం *p;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(క్షేత్రం);

        ఒకవేళ (ordered_సమితి_isempty(s))
                తిరిగి;

        fputs(క్షేత్రం, f);

        ORDERED_సమితి_FOREACH(p, s)
                fputs_with_separator(f, p, ఖాళీ, &space);

        fputc('\n', f);
}
