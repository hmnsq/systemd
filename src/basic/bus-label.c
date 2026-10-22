/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bus-label.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਅੱਖਰ* bus_label_escape(ਅਚਲ ਅੱਖਰ *s) {
        ਅੱਖਰ *r, *t;
        ਅਚਲ ਅੱਖਰ *f;

        assert_return(s, ਨਿਰਾ);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        ਜੇ (*s == 0)
                ਵਾਪਸ strdup("_");

        r = new(ਅੱਖਰ, ਲੜੀ_ਲੰਬਾਈ(s)*3 + 1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ਜੇ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ਨਹੀਂਤਾਂ
                        *(t++) = *f;
        }

        *t = 0;

        ਵਾਪਸ r;
}

ਅੱਖਰ* bus_label_unescape_n(ਅਚਲ ਅੱਖਰ *f, size_t l) {
        ਅੱਖਰ *r, *t;
        size_t i;

        assert_return(f, ਨਿਰਾ);

        ਜੇ (l == SIZE_MAX)
                l = ਲੜੀ_ਲੰਬਾਈ(f);

        /* Special case for the empty string */
        ਜੇ (l == 1 && *f == '_')
                ਵਾਪਸ strdup("");

        r = new(ਅੱਖਰ, l + 1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (i = 0, t = r; i < l; ++i) {
                ਜੇ (f[i] == '_') {
                        ਪੂਰਨ a, b;

                        ਜੇ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ਨਹੀਂਤਾਂ {
                                *(t++) = (ਅੱਖਰ) ((a << 4) | b);
                                i += 2;
                        }
                } ਨਹੀਂਤਾਂ
                        *(t++) = f[i];
        }

        *t = 0;

        ਵਾਪਸ r;
}
