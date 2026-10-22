/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "ordered-set.h"
#അന്തര്ഭാവ "strv.h"

പൂർണ്ണസംഖ്യ ordered_set_ensure_allocated(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *ops) {
        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (*s)
                തിരികെ 0;

        *s = ordered_set_new(ops);
        എങ്കിൽ (!*s)
                തിരികെ -ENOMEM;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ ordered_set_ensure_put(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *ops, ശൂന്യം *p) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        r = ordered_set_ensure_allocated(s, ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ ordered_set_put(*s, p);
}

പൂർണ്ണസംഖ്യ ordered_set_consume(OrderedSet *s, ശൂന്യം *p) {
        പൂർണ്ണസംഖ്യ r;

        r = ordered_set_put(s, p);
        എങ്കിൽ (r <= 0)
                സ്വതന്ത്രം(p);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ ordered_set_put_strdup_full(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം അക്ഷരം *p) {
        അക്ഷരം *c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(p);

        r = ordered_set_ensure_allocated(s, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ordered_set_contains(*s, p))
                തിരികെ 0;

        c = strdup(p);
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        തിരികെ ordered_set_consume(*s, c);
}

പൂർണ്ണസംഖ്യ ordered_set_put_strdupv_full(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *hash_ops, അക്ഷരം **l) {
        പൂർണ്ണസംഖ്യ r, ret = 0;

        പ്രതിജ്ഞാ(s);

        STRV_FOREACH(i, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, *i);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ret = ret || r > 0;
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ ordered_set_put_string_set_full(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *hash_ops, OrderedSet *l) {
        പൂർണ്ണസംഖ്യ r, ret = 0;
        അക്ഷരം *p;

        പ്രതിജ്ഞാ(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_SET_FOREACH(p, l) {
                r = ordered_set_put_strdup_full(s, hash_ops, p);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ret = ret || r > 0;
        }

        തിരികെ ret;
}

ശൂന്യം ordered_set_print(FILE *f, സ്ഥിരം അക്ഷരം *field, OrderedSet *s) {
        bool space = false;
        അക്ഷരം *p;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(field);

        എങ്കിൽ (ordered_set_isempty(s))
                തിരികെ;

        fputs(field, f);

        ORDERED_SET_FOREACH(p, s)
                fputs_with_separator(f, p, ശൂന്യ, &space);

        fputc('\n', f);
}
