/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "bus-label.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "string-util.h"

અક્ષર* bus_label_escape(અચલ અક્ષર *s) {
        અક્ષર *r, *t;
        અચલ અક્ષર *f;

        assert_return(s, ખાલી);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        જો (*s == 0)
                પાછા strdup("_");

        r = new(અક્ષર, strlen(s)*3 + 1);
        જો (!r)
                પાછા ખાલી;

        માટે (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                જો (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } નહીંતર
                        *(t++) = *f;
        }

        *t = 0;

        પાછા r;
}

અક્ષર* bus_label_unescape_n(અચલ અક્ષર *f, size_t l) {
        અક્ષર *r, *t;
        size_t i;

        assert_return(f, ખાલી);

        જો (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        જો (l == 1 && *f == '_')
                પાછા strdup("");

        r = new(અક્ષર, l + 1);
        જો (!r)
                પાછા ખાલી;

        માટે (i = 0, t = r; i < l; ++i) {
                જો (f[i] == '_') {
                        પૂર્ણાંક a, b;

                        જો (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } નહીંતર {
                                *(t++) = (અક્ષર) ((a << 4) | b);
                                i += 2;
                        }
                } નહીંતર
                        *(t++) = f[i];
        }

        *t = 0;

        પાછા r;
}
