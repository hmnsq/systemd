/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bus-label.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ವರ್ಣ* bus_label_escape(ಧ್ರುವ ವರ್ಣ *s) {
        ವರ್ಣ *r, *t;
        ಧ್ರುವ ವರ್ಣ *f;

        assert_return(s, ರಿಕ್ತ);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        ಯದಿ (*s == 0)
                ಪ್ರತ್ಯಾಗಮ strdup("_");

        r = new(ವರ್ಣ, strlen(s)*3 + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ಯದಿ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ಅನ್ಯಥಾ
                        *(t++) = *f;
        }

        *t = 0;

        ಪ್ರತ್ಯಾಗಮ r;
}

ವರ್ಣ* bus_label_unescape_n(ಧ್ರುವ ವರ್ಣ *f, size_t l) {
        ವರ್ಣ *r, *t;
        size_t i;

        assert_return(f, ರಿಕ್ತ);

        ಯದಿ (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        ಯದಿ (l == 1 && *f == '_')
                ಪ್ರತ್ಯಾಗಮ strdup("");

        r = new(ವರ್ಣ, l + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (i = 0, t = r; i < l; ++i) {
                ಯದಿ (f[i] == '_') {
                        ಪೂರ್ಣಾಙ್ಕ a, b;

                        ಯದಿ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ಅನ್ಯಥಾ {
                                *(t++) = (ವರ್ಣ) ((a << 4) | b);
                                i += 2;
                        }
                } ಅನ್ಯಥಾ
                        *(t++) = f[i];
        }

        *t = 0;

        ಪ್ರತ್ಯಾಗಮ r;
}
