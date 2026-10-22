/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "ordered-set.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_ensure_allocated(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *ops) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (*s)
                ಹಿಂತಿರುಗಿ 0;

        *s = ordered_ಗಣ_new(ops);
        ಆದರೆ (!*s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_ensure_put(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *ops, ಶೂನ್ಯ *p) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ordered_ಗಣ_ensure_allocated(s, ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ordered_ಗಣ_put(*s, p);
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_consume(Ordered_ಗಣ *s, ಶೂನ್ಯ *p) {
        ಪೂರ್ಣಾಂಕ r;

        r = ordered_ಗಣ_put(s, p);
        ಆದರೆ (r <= 0)
                ಬಿಡುಗಡೆ(p);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put_strdup_ತುಂಬಿದ(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಅಕ್ಷರ *c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(p);

        r = ordered_ಗಣ_ensure_allocated(s, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ordered_ಗಣ_ಒಳಗೊಂಡಿದೆ(*s, p))
                ಹಿಂತಿರುಗಿ 0;

        c = strdup(p);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ ordered_ಗಣ_consume(*s, c);
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put_strdupv_ತುಂಬಿದ(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಅಕ್ಷರ **l) {
        ಪೂರ್ಣಾಂಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(s);

        STRV_FOREACH(i, l) {
                r = ordered_ಗಣ_put_strdup_ತುಂಬಿದ(s, hash_ops, *i);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ret = ret || r > 0;
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put_string_ಗಣ_ತುಂಬಿದ(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, Ordered_ಗಣ *l) {
        ಪೂರ್ಣಾಂಕ r, ret = 0;
        ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Like ordered_set_put_strv, but for an OrderedSet of strings */

        ORDERED_ಗಣ_FOREACH(p, l) {
                r = ordered_ಗಣ_put_strdup_ತುಂಬಿದ(s, hash_ops, p);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ret = ret || r > 0;
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ ordered_ಗಣ_print(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, Ordered_ಗಣ *s) {
        bool space = false;
        ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ಕ್ಷೇತ್ರ);

        ಆದರೆ (ordered_ಗಣ_isempty(s))
                ಹಿಂತಿರುಗಿ;

        fputs(ಕ್ಷೇತ್ರ, f);

        ORDERED_ಗಣ_FOREACH(p, s)
                fputs_with_separator(f, p, ಖಾಲಿ, &space);

        fputc('\n', f);
}
