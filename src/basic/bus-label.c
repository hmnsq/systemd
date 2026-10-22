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

        если (*s == 0)
                вернуть strdup("_");

        r = new(char, strlen(s)*3 + 1);
        если (!r)
                вернуть NULL;

        для (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                если (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } иначе
                        *(t++) = *f;
        }

        *t = 0;

        вернуть r;
}

char* bus_label_unescape_n(const char *f, size_t l) {
        char *r, *t;
        size_t i;

        assert_return(f, NULL);

        если (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        если (l == 1 && *f == '_')
                вернуть strdup("");

        r = new(char, l + 1);
        если (!r)
                вернуть NULL;

        для (i = 0, t = r; i < l; ++i) {
                если (f[i] == '_') {
                        целое a, b;

                        если (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } иначе {
                                *(t++) = (char) ((a << 4) | b);
                                i += 2;
                        }
                } иначе
                        *(t++) = f[i];
        }

        *t = 0;

        вернуть r;
}
