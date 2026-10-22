/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "fileio.h"
#ضمن "ordered-set.h"
#ضمن "strv.h"

صحيح ordered_مجموعه_ensure_allocated(Ordered_مجموعه **s, ثابت بنية hash_ops *ops) {
        أكد(s);

        إذا (*s)
                أرجع 0;

        *s = ordered_مجموعه_new(ops);
        إذا (!*s)
                أرجع -ENOMEM;

        أرجع 0;
}

صحيح ordered_مجموعه_ensure_put(Ordered_مجموعه **s, ثابت بنية hash_ops *ops, فراغ *p) {
        صحيح r;

        أكد(s);

        r = ordered_مجموعه_ensure_allocated(s, ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_مجموعه_put(*s, p);
}

صحيح ordered_مجموعه_consume(Ordered_مجموعه *s, فراغ *p) {
        صحيح r;

        r = ordered_مجموعه_put(s, p);
        إذا (r <= 0)
                حرر(p);

        أرجع r;
}

صحيح ordered_مجموعه_put_strdup_پر(Ordered_مجموعه **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = ordered_مجموعه_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (ordered_مجموعه_شامل_است(*s, p))
                أرجع 0;

        c = strdup(p);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع ordered_مجموعه_consume(*s, c);
}

صحيح ordered_مجموعه_put_strdupv_پر(Ordered_مجموعه **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح r, ret = 0;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = ordered_مجموعه_put_strdup_پر(s, hash_ops, *i);
                إذا (r < 0)
                        أرجع r;

                ret = ret || r > 0;
        }

        أرجع ret;
}

صحيح ordered_مجموعه_put_string_مجموعه_پر(Ordered_مجموعه **s, ثابت بنية hash_ops *hash_ops, Ordered_مجموعه *l) {
        صحيح r, ret = 0;
        حرف *p;

        أكد(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_مجموعه_FOREACH(p, l) {
                r = ordered_مجموعه_put_strdup_پر(s, hash_ops, p);
                إذا (r < 0)
                        أرجع r;

                ret = ret || r > 0;
        }

        أرجع ret;
}

فراغ ordered_مجموعه_print(FILE *f, ثابت حرف *فیلد, Ordered_مجموعه *s) {
        bool space = false;
        حرف *p;

        أكد(f);
        أكد(فیلد);

        إذا (ordered_مجموعه_isempty(s))
                أرجع;

        fputs(فیلد, f);

        ORDERED_مجموعه_FOREACH(p, s)
                fputs_with_separator(f, p, عدم, &space);

        fputc('\n', f);
}
