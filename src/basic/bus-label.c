/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "bus-label.h"
#ضمن "hexdecoct.h"
#ضمن "string-util.h"

حرف* bus_label_escape(ثابت حرف *s) {
        حرف *r, *t;
        ثابت حرف *f;

        assert_return(s, عدم);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        إذا (*s == 0)
                أرجع strdup("_");

        r = new(حرف, لڑی_لمبائی(s)*3 + 1);
        إذا (!r)
                أرجع عدم;

        لكل (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                إذا (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } وإلا
                        *(t++) = *f;
        }

        *t = 0;

        أرجع r;
}

حرف* bus_label_unescape_n(ثابت حرف *f, size_t l) {
        حرف *r, *t;
        size_t i;

        assert_return(f, عدم);

        إذا (l == SIZE_MAX)
                l = لڑی_لمبائی(f);

        /* Special case for the empty string */
        إذا (l == 1 && *f == '_')
                أرجع strdup("");

        r = new(حرف, l + 1);
        إذا (!r)
                أرجع عدم;

        لكل (i = 0, t = r; i < l; ++i) {
                إذا (f[i] == '_') {
                        صحيح a, b;

                        إذا (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } وإلا {
                                *(t++) = (حرف) ((a << 4) | b);
                                i += 2;
                        }
                } وإلا
                        *(t++) = f[i];
        }

        *t = 0;

        أرجع r;
}
