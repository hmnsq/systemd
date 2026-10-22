/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "alloc-util.h"
#include "bus-label.h"
#include "hexdecoct.h"
#include "string-util.h"

var_na* bus_label_escape(const var_na *s) {
        var_na *r, *t;
        const var_na *f;

        assert_return(s, NULL);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        ᯀᯮᯝ (*s == 0)
                ᯀᯮᯞ strdup("_");

        r = new(var_na, strlen(s)*3 + 1);
        ᯀᯮᯝ (!r)
                ᯀᯮᯞ NULL;

        for (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ᯀᯮᯝ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ᯀᯮᯝᯂᯪ
                        *(t++) = *f;
        }

        *t = 0;

        ᯀᯮᯞ r;
}

var_na* bus_label_unescape_n(const var_na *f, size_t l) {
        var_na *r, *t;
        size_t i;

        assert_return(f, NULL);

        ᯀᯮᯝ (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        ᯀᯮᯝ (l == 1 && *f == '_')
                ᯀᯮᯞ strdup("");

        r = new(var_na, l + 1);
        ᯀᯮᯝ (!r)
                ᯀᯮᯞ NULL;

        for (i = 0, t = r; i < l; ++i) {
                ᯀᯮᯝ (f[i] == '_') {
                        puur_naa_nka a, b;

                        ᯀᯮᯝ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ᯀᯮᯝᯂᯪ {
                                *(t++) = (var_na) ((a << 4) | b);
                                i += 2;
                        }
                } ᯀᯮᯝᯂᯪ
                        *(t++) = f[i];
        }

        *t = 0;

        ᯀᯮᯞ r;
}
