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

        གལ་ཏེ (*s == 0)
                ཕྱིར strdup("_");

        r = new(var_na, strlen(s)*3 + 1);
        གལ་ཏེ (!r)
                ཕྱིར NULL;

        ཆེད་དུ (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                གལ་ཏེ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } དེ་མིན
                        *(t++) = *f;
        }

        *t = 0;

        ཕྱིར r;
}

var_na* bus_label_unescape_n(const var_na *f, size_t l) {
        var_na *r, *t;
        size_t i;

        assert_return(f, NULL);

        གལ་ཏེ (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        གལ་ཏེ (l == 1 && *f == '_')
                ཕྱིར strdup("");

        r = new(var_na, l + 1);
        གལ་ཏེ (!r)
                ཕྱིར NULL;

        ཆེད་དུ (i = 0, t = r; i < l; ++i) {
                གལ་ཏེ (f[i] == '_') {
                        ཧྲིལ a, b;

                        གལ་ཏེ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } དེ་མིན {
                                *(t++) = (var_na) ((a << 4) | b);
                                i += 2;
                        }
                } དེ་མིན
                        *(t++) = f[i];
        }

        *t = 0;

        ཕྱིར r;
}
