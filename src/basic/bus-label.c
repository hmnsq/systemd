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

        もし (*s == 0)
                戻る strdup("_");

        r = new(char, strlen(s)*3 + 1);
        もし (!r)
                戻る NULL;

        ために (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                もし (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } 否则
                        *(t++) = *f;
        }

        *t = 0;

        戻る r;
}

char* bus_label_unescape_n(const char *f, size_t l) {
        char *r, *t;
        size_t i;

        assert_return(f, NULL);

        もし (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        もし (l == 1 && *f == '_')
                戻る strdup("");

        r = new(char, l + 1);
        もし (!r)
                戻る NULL;

        ために (i = 0, t = r; i < l; ++i) {
                もし (f[i] == '_') {
                        整数 a, b;

                        もし (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } 否则 {
                                *(t++) = (char) ((a << 4) | b);
                                i += 2;
                        }
                } 否则
                        *(t++) = f[i];
        }

        *t = 0;

        戻る r;
}
