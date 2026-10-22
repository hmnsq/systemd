/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

පූර්ණ_සංඛ්‍යා ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops) {
        assert(s);

        නම් (*s)
                ආපසු 0;

        *s = ordered_set_new(ops);
        නම් (!*s)
                ආපසු -ENOMEM;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p) {
        පූර්ණ_සංඛ්‍යා r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        නම් (r < 0)
                ආපසු r;

        ආපසු ordered_set_put(*s, p);
}

පූර්ණ_සංඛ්‍යා ordered_set_consume(OrderedSet *s, void *p) {
        පූර්ණ_සංඛ්‍යා r;

        r = ordered_set_put(s, p);
        නම් (r <= 0)
                free(p);

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p) {
        var_na *c;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        නම් (r < 0)
                ආපසු r;

        නම් (ordered_set_contains(*s, p))
                ආපසු 0;

        c = strdup(p);
        නම් (!c)
                ආපසු -ENOMEM;

        ආපසු ordered_set_consume(*s, c);
}

පූර්ණ_සංඛ්‍යා ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        පූර්ණ_සංඛ්‍යා r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                නම් (r < 0)
                        ආපසු r;

                ret = ret || r > 0;
        }

        ආපසු ret;
}

පූර්ණ_සංඛ්‍යා ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l) {
        පූර්ණ_සංඛ්‍යා r, ret = 0;
        var_na *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                නම් (r < 0)
                        ආපසු r;

                ret = ret || r > 0;
        }

        ආපසු ret;
}

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s) {
        bool space = false;
        var_na *p;

        assert(f);
        assert(field);

        නම් (ordered_set_isempty(s))
                ආපසු;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
