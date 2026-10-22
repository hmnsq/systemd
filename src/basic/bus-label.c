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

        ຖ້າ (*s == 0)
                ສົ່ງຄືນ strdup("_");

        r = new(var_na, strlen(s)*3 + 1);
        ຖ້າ (!r)
                ສົ່ງຄືນ NULL;

        ສຳລັບ (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ຖ້າ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ບໍ່ດັ່ງນັ້ນ
                        *(t++) = *f;
        }

        *t = 0;

        ສົ່ງຄືນ r;
}

var_na* bus_label_unescape_n(const var_na *f, size_t l) {
        var_na *r, *t;
        size_t i;

        assert_return(f, NULL);

        ຖ້າ (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        ຖ້າ (l == 1 && *f == '_')
                ສົ່ງຄືນ strdup("");

        r = new(var_na, l + 1);
        ຖ້າ (!r)
                ສົ່ງຄືນ NULL;

        ສຳລັບ (i = 0, t = r; i < l; ++i) {
                ຖ້າ (f[i] == '_') {
                        ຈຳນວນເຕັມ a, b;

                        ຖ້າ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ບໍ່ດັ່ງນັ້ນ {
                                *(t++) = (var_na) ((a << 4) | b);
                                i += 2;
                        }
                } ບໍ່ດັ່ງນັ້ນ
                        *(t++) = f[i];
        }

        *t = 0;

        ສົ່ງຄືນ r;
}
