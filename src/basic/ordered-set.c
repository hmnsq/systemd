/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "ordered-set.h"
#include "strv.h"

puur_naa_nka ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops) {
        assert(s);

        ᨀᨘ (*s)
                ᨀᨘᨒ 0;

        *s = ordered_set_new(ops);
        ᨀᨘ (!*s)
                ᨀᨘᨒ -ENOMEM;

        ᨀᨘᨒ 0;
}

puur_naa_nka ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p) {
        puur_naa_nka r;

        assert(s);

        r = ordered_set_ensure_allocated(s, ops);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ ordered_set_put(*s, p);
}

puur_naa_nka ordered_set_consume(OrderedSet *s, void *p) {
        puur_naa_nka r;

        r = ordered_set_put(s, p);
        ᨀᨘ (r <= 0)
                free(p);

        ᨀᨘᨒ r;
}

puur_naa_nka ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p) {
        var_na *c;
        puur_naa_nka r;

        assert(s);
        assert(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘ (ordered_set_contains(*s, p))
                ᨀᨘᨒ 0;

        c = strdup(p);
        ᨀᨘ (!c)
                ᨀᨘᨒ -ENOMEM;

        ᨀᨘᨒ ordered_set_consume(*s, c);
}

puur_naa_nka ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        puur_naa_nka r, ret = 0;

        assert(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                ret = ret || r > 0;
        }

        ᨀᨘᨒ ret;
}

puur_naa_nka ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l) {
        puur_naa_nka r, ret = 0;
        var_na *p;

        assert(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                ret = ret || r > 0;
        }

        ᨀᨘᨒ ret;
}

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s) {
        bool space = false;
        var_na *p;

        assert(f);
        assert(field);

        ᨀᨘ (ordered_set_isempty(s))
                ᨀᨘᨒ;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, NULL, &space);

        fputc('\n', f);
}
