/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "fileio.h"
#ضمن "ordered-set.h"
#ضمن "strv.h"

صحيح ordered_سيٽ_ensure_allocated(Ordered_سيٽ **s, ثابت بنية hash_ops *ops) {
        أكد(s);

        إذا (*s)
                أرجع 0;

        *s = ordered_سيٽ_new(ops);
        إذا (!*s)
                أرجع -ENOMEM;

        أرجع 0;
}

صحيح ordered_سيٽ_ensure_put(Ordered_سيٽ **s, ثابت بنية hash_ops *ops, فراغ *p) {
        صحيح r;

        أكد(s);

        r = ordered_سيٽ_ensure_allocated(s, ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_سيٽ_put(*s, p);
}

صحيح ordered_سيٽ_consume(Ordered_سيٽ *s, فراغ *p) {
        صحيح r;

        r = ordered_سيٽ_put(s, p);
        إذا (r <= 0)
                حرر(p);

        أرجع r;
}

صحيح ordered_سيٽ_put_strdup_ڀريل(Ordered_سيٽ **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = ordered_سيٽ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (ordered_سيٽ_شامل_آهي(*s, p))
                أرجع 0;

        c = strdup(p);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع ordered_سيٽ_consume(*s, c);
}

صحيح ordered_سيٽ_put_strdupv_ڀريل(Ordered_سيٽ **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح r, ret = 0;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = ordered_سيٽ_put_strdup_ڀريل(s, hash_ops, *i);
                إذا (r < 0)
                        أرجع r;

                ret = ret || r > 0;
        }

        أرجع ret;
}

صحيح ordered_سيٽ_put_string_سيٽ_ڀريل(Ordered_سيٽ **s, ثابت بنية hash_ops *hash_ops, Ordered_سيٽ *l) {
        صحيح r, ret = 0;
        حرف *p;

        أكد(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_سيٽ_FOREACH(p, l) {
                r = ordered_سيٽ_put_strdup_ڀريل(s, hash_ops, p);
                إذا (r < 0)
                        أرجع r;

                ret = ret || r > 0;
        }

        أرجع ret;
}

فراغ ordered_سيٽ_print(FILE *f, ثابت حرف *ميدان, Ordered_سيٽ *s) {
        bool space = false;
        حرف *p;

        أكد(f);
        أكد(ميدان);

        إذا (ordered_سيٽ_isempty(s))
                أرجع;

        fputs(ميدان, f);

        ORDERED_سيٽ_FOREACH(p, s)
                fputs_with_separator(f, p, عدم, &space);

        fputc('\n', f);
}
