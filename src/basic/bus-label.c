/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "alloc-util.h"
#include "bus-label.h"
#include "hexdecoct.h"
#include "string-util.h"

char* bus_label_escape(const char *s) {
        char *r, *t;
        const char *f;

        assert_return(s, NULL);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        nếu (*s == 0)
                trả_về strdup("_");

        r = new(char, strlen(s)*3 + 1);
        nếu (!r)
                trả_về NULL;

        cho (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                nếu (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } không_thì
                        *(t++) = *f;
        }

        *t = 0;

        trả_về r;
}

char* bus_label_unescape_n(const char *f, size_t l) {
        char *r, *t;
        size_t i;

        assert_return(f, NULL);

        nếu (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        nếu (l == 1 && *f == '_')
                trả_về strdup("");

        r = new(char, l + 1);
        nếu (!r)
                trả_về NULL;

        cho (i = 0, t = r; i < l; ++i) {
                nếu (f[i] == '_') {
                        số_nguyên a, b;

                        nếu (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } không_thì {
                                *(t++) = (char) ((a << 4) | b);
                                i += 2;
                        }
                } không_thì
                        *(t++) = f[i];
        }

        *t = 0;

        trả_về r;
}
