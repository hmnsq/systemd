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

        ܐܢ (*s == 0)
                ܦܢܐ strdup("_");

        r = new(حرف, strlen(s)*3 + 1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        لكل (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ܐܢ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ܐܚܪܢܐ
                        *(t++) = *f;
        }

        *t = 0;

        ܦܢܐ r;
}

حرف* bus_label_unescape_n(ثابت حرف *f, size_t l) {
        حرف *r, *t;
        size_t i;

        assert_return(f, عدم);

        ܐܢ (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        ܐܢ (l == 1 && *f == '_')
                ܦܢܐ strdup("");

        r = new(حرف, l + 1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        لكل (i = 0, t = r; i < l; ++i) {
                ܐܢ (f[i] == '_') {
                        صحيح a, b;

                        ܐܢ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ܐܚܪܢܐ {
                                *(t++) = (حرف) ((a << 4) | b);
                                i += 2;
                        }
                } ܐܚܪܢܐ
                        *(t++) = f[i];
        }

        *t = 0;

        ܦܢܐ r;
}
