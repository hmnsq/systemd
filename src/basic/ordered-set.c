/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ordered-set.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਪੂਰਨ ordered_ਸੈੱਟ_ensure_allocated(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *ops) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (*s)
                ਵਾਪਸ 0;

        *s = ordered_ਸੈੱਟ_new(ops);
        ਜੇ (!*s)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ordered_ਸੈੱਟ_ensure_put(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *ops, ਖਾਲੀ *p) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ordered_ਸੈੱਟ_ensure_allocated(s, ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ordered_ਸੈੱਟ_put(*s, p);
}

ਪੂਰਨ ordered_ਸੈੱਟ_consume(Ordered_ਸੈੱਟ *s, ਖਾਲੀ *p) {
        ਪੂਰਨ r;

        r = ordered_ਸੈੱਟ_put(s, p);
        ਜੇ (r <= 0)
                ਮੁਕਤ(p);

        ਵਾਪਸ r;
}

ਪੂਰਨ ordered_ਸੈੱਟ_put_strdup_ਭਰਿਆ(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਅੱਖਰ *p) {
        ਅੱਖਰ *c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        r = ordered_ਸੈੱਟ_ensure_allocated(s, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ordered_ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(*s, p))
                ਵਾਪਸ 0;

        c = strdup(p);
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ ordered_ਸੈੱਟ_consume(*s, c);
}

ਪੂਰਨ ordered_ਸੈੱਟ_put_strdupv_ਭਰਿਆ(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅੱਖਰ **l) {
        ਪੂਰਨ r, ret = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        STRV_FOREACH(i, l) {
                r = ordered_ਸੈੱਟ_put_strdup_ਭਰਿਆ(s, hash_ops, *i);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ret = ret || r > 0;
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ ordered_ਸੈੱਟ_put_string_ਸੈੱਟ_ਭਰਿਆ(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, Ordered_ਸੈੱਟ *l) {
        ਪੂਰਨ r, ret = 0;
        ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_ਸੈੱਟ_FOREACH(p, l) {
                r = ordered_ਸੈੱਟ_put_strdup_ਭਰਿਆ(s, hash_ops, p);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ret = ret || r > 0;
        }

        ਵਾਪਸ ret;
}

ਖਾਲੀ ordered_ਸੈੱਟ_print(FILE *f, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, Ordered_ਸੈੱਟ *s) {
        bool space = false;
        ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਖੇਤਰ);

        ਜੇ (ordered_ਸੈੱਟ_isempty(s))
                ਵਾਪਸ;

        fputs(ਖੇਤਰ, f);

        ORDERED_ਸੈੱਟ_FOREACH(p, s)
                fputs_with_separator(f, p, ਨਿਰਾ, &space);

        fputc('\n', f);
}
