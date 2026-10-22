/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "bus-label.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "string-util.h"

எழுத்து* bus_label_escape(மாறிலி எழுத்து *s) {
        எழுத்து *r, *t;
        மாறிலி எழுத்து *f;

        assert_return(s, வெற்று);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        எனில் (*s == 0)
                திரும்பு strdup("_");

        r = new(எழுத்து, strlen(s)*3 + 1);
        எனில் (!r)
                திரும்பு வெற்று;

        க்கு (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                எனில் (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } இல்லையெனில்
                        *(t++) = *f;
        }

        *t = 0;

        திரும்பு r;
}

எழுத்து* bus_label_unescape_n(மாறிலி எழுத்து *f, size_t l) {
        எழுத்து *r, *t;
        size_t i;

        assert_return(f, வெற்று);

        எனில் (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        எனில் (l == 1 && *f == '_')
                திரும்பு strdup("");

        r = new(எழுத்து, l + 1);
        எனில் (!r)
                திரும்பு வெற்று;

        க்கு (i = 0, t = r; i < l; ++i) {
                எனில் (f[i] == '_') {
                        முழு a, b;

                        எனில் (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } இல்லையெனில் {
                                *(t++) = (எழுத்து) ((a << 4) | b);
                                i += 2;
                        }
                } இல்லையெனில்
                        *(t++) = f[i];
        }

        *t = 0;

        திரும்பு r;
}
