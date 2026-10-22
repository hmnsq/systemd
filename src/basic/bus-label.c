/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "bus-label.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ଅକ୍ଷର* bus_label_escape(ଅଚଳ ଅକ୍ଷର *s) {
        ଅକ୍ଷର *r, *t;
        ଅଚଳ ଅକ୍ଷର *f;

        assert_return(s, ଖାଲି);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        ଯଦି (*s == 0)
                ଫେରନ୍ତୁ strdup("_");

        r = new(ଅକ୍ଷର, strlen(s)*3 + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ଯଦି (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ଅନ୍ୟଥା
                        *(t++) = *f;
        }

        *t = 0;

        ଫେରନ୍ତୁ r;
}

ଅକ୍ଷର* bus_label_unescape_n(ଅଚଳ ଅକ୍ଷର *f, size_t l) {
        ଅକ୍ଷର *r, *t;
        size_t i;

        assert_return(f, ଖାଲି);

        ଯଦି (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        ଯଦି (l == 1 && *f == '_')
                ଫେରନ୍ତୁ strdup("");

        r = new(ଅକ୍ଷର, l + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (i = 0, t = r; i < l; ++i) {
                ଯଦି (f[i] == '_') {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b;

                        ଯଦି (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } ଅନ୍ୟଥା {
                                *(t++) = (ଅକ୍ଷର) ((a << 4) | b);
                                i += 2;
                        }
                } ଅନ୍ୟଥା
                        *(t++) = f[i];
        }

        *t = 0;

        ଫେରନ୍ତୁ r;
}
