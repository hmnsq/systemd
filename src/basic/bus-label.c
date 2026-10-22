/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "bus-label.h"
#شامل "hexdecoct.h"
#شامل "string-util.h"

نویسه* bus_label_escape(ثابت نویسه *s) {
        نویسه *r, *t;
        ثابت نویسه *f;

        assert_return(s, پوچ);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        اگر (*s == 0)
                بازگشت strdup("_");

        r = new(نویسه, strlen(s)*3 + 1);
        اگر (!r)
                بازگشت پوچ;

        برای (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                اگر (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } وگرنه
                        *(t++) = *f;
        }

        *t = 0;

        بازگشت r;
}

نویسه* bus_label_unescape_n(ثابت نویسه *f, size_t l) {
        نویسه *r, *t;
        size_t i;

        assert_return(f, پوچ);

        اگر (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        اگر (l == 1 && *f == '_')
                بازگشت strdup("");

        r = new(نویسه, l + 1);
        اگر (!r)
                بازگشت پوچ;

        برای (i = 0, t = r; i < l; ++i) {
                اگر (f[i] == '_') {
                        صحیح a, b;

                        اگر (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } وگرنه {
                                *(t++) = (نویسه) ((a << 4) | b);
                                i += 2;
                        }
                } وگرنه
                        *(t++) = f[i];
        }

        *t = 0;

        بازگشت r;
}
