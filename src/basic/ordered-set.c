/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "ordered-set.h"
#அந்தர்பாவ "strv.h"

முழு ordered_set_ensure_allocated(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *ops) {
        ப்ரதிஜ்ஞா(s);

        எனில் (*s)
                திரும்பு 0;

        *s = ordered_set_new(ops);
        எனில் (!*s)
                திரும்பு -ENOMEM;

        திரும்பு 0;
}

முழு ordered_set_ensure_put(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *ops, வெறுமை *p) {
        முழு r;

        ப்ரதிஜ்ஞா(s);

        r = ordered_set_ensure_allocated(s, ops);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு ordered_set_put(*s, p);
}

முழு ordered_set_consume(OrderedSet *s, வெறுமை *p) {
        முழு r;

        r = ordered_set_put(s, p);
        எனில் (r <= 0)
                விடுவி(p);

        திரும்பு r;
}

முழு ordered_set_put_strdup_full(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி எழுத்து *p) {
        எழுத்து *c;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ordered_set_contains(*s, p))
                திரும்பு 0;

        c = strdup(p);
        எனில் (!c)
                திரும்பு -ENOMEM;

        திரும்பு ordered_set_consume(*s, c);
}

முழு ordered_set_put_strdupv_full(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, எழுத்து **l) {
        முழு r, ret = 0;

        ப்ரதிஜ்ஞா(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                எனில் (r < 0)
                        திரும்பு r;

                ret = ret || r > 0;
        }

        திரும்பு ret;
}

முழு ordered_set_put_string_set_full(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, OrderedSet *l) {
        முழு r, ret = 0;
        எழுத்து *p;

        ப்ரதிஜ்ஞா(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                எனில் (r < 0)
                        திரும்பு r;

                ret = ret || r > 0;
        }

        திரும்பு ret;
}

வெறுமை ordered_set_print(FILE *f, மாறிலி எழுத்து *field, OrderedSet *s) {
        bool space = false;
        எழுத்து *p;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(field);

        எனில் (ordered_set_isempty(s))
                திரும்பு;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, வெற்று, &space);

        fputc('\n', f);
}
