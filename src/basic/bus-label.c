/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "bus-label.h"
#شامل "hexdecoct.h"
#شامل "string-util.h"

حرف* bus_label_escape(ثابت حرف *s) {
        حرف *r, *t;
        ثابت حرف *f;

        assert_return(s, تہی);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        اگر (*s == 0)
                واپس strdup("_");

        r = new(حرف, لڑی_لمبائی(s)*3 + 1);
        اگر (!r)
                واپس تہی;

        برائے (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                اگر (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ورنہ
                        *(t++) = *f;
        }

        *t = 0;

        واپس r;
}

حرف* bus_label_unescape_n(ثابت حرف *f, size_t l) {
        حرف *r, *t;
        size_t i;

        assert_return(f, تہی);

        اگر (l == SIZE_MAX)
                l = لڑی_لمبائی(f);

        /* Special case for the empty string */
        اگر (l == 1 && *f == '_')
                واپس strdup("");

        r = new(حرف, l + 1);
        اگر (!r)
                واپس تہی;

        برائے (i = 0, t = r; i < l; ++i) {
                اگر (f[i] == '_') {
                        صحیح a, b;

                        اگر (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ورنہ {
                                *(t++) = (حرف) ((a << 4) | b);
                                i += 2;
                        }
                } ورنہ
                        *(t++) = f[i];
        }

        *t = 0;

        واپس r;
}
