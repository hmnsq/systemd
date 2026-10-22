/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bus-label.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಅಕ್ಷರ* bus_label_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಅಕ್ಷರ *r, *t;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;

        assert_return(s, ಖಾಲಿ);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        ಆದರೆ (*s == 0)
                ಹಿಂತಿರುಗಿ strdup("_");

        r = new(ಅಕ್ಷರ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s)*3 + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ಆದರೆ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = *f;
        }

        *t = 0;

        ಹಿಂತಿರುಗಿ r;
}

ಅಕ್ಷರ* bus_label_unescape_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, size_t l) {
        ಅಕ್ಷರ *r, *t;
        size_t i;

        assert_return(f, ಖಾಲಿ);

        ಆದರೆ (l == SIZE_MAX)
                l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(f);

        /* Special case for the empty string */
        ಆದರೆ (l == 1 && *f == '_')
                ಹಿಂತಿರುಗಿ strdup("");

        r = new(ಅಕ್ಷರ, l + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (i = 0, t = r; i < l; ++i) {
                ಆದರೆ (f[i] == '_') {
                        ಪೂರ್ಣಾಂಕ a, b;

                        ಆದರೆ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                *(t++) = (ಅಕ್ಷರ) ((a << 4) | b);
                                i += 2;
                        }
                } ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = f[i];
        }

        *t = 0;

        ಹಿಂತಿರುಗಿ r;
}
