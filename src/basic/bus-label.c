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

        만약 (*s == 0)
                반환 strdup("_");

        r = new(char, strlen(s)*3 + 1);
        만약 (!r)
                반환 NULL;

        위해 (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                만약 (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } 아니면
                        *(t++) = *f;
        }

        *t = 0;

        반환 r;
}

char* bus_label_unescape_n(const char *f, size_t l) {
        char *r, *t;
        size_t i;

        assert_return(f, NULL);

        만약 (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        만약 (l == 1 && *f == '_')
                반환 strdup("");

        r = new(char, l + 1);
        만약 (!r)
                반환 NULL;

        위해 (i = 0, t = r; i < l; ++i) {
                만약 (f[i] == '_') {
                        정수 a, b;

                        만약 (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } 아니면 {
                                *(t++) = (char) ((a << 4) | b);
                                i += 2;
                        }
                } 아니면
                        *(t++) = f[i];
        }

        *t = 0;

        반환 r;
}
