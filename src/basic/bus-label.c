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

        如果 (*s == 0)
                返回 strdup("_");

        r = new(char, strlen(s)*3 + 1);
        如果 (!r)
                返回 NULL;

        为 (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                如果 (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } 否则
                        *(t++) = *f;
        }

        *t = 0;

        返回 r;
}

char* bus_label_unescape_n(const char *f, size_t l) {
        char *r, *t;
        size_t i;

        assert_return(f, NULL);

        如果 (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        如果 (l == 1 && *f == '_')
                返回 strdup("");

        r = new(char, l + 1);
        如果 (!r)
                返回 NULL;

        为 (i = 0, t = r; i < l; ++i) {
                如果 (f[i] == '_') {
                        整数 a, b;

                        如果 (l - i < 3 ||
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

        返回 r;
}
