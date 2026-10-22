/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdlib.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "bus-label.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "string-util.h"

ৱর্ণ* bus_label_escape(ধ্রুৱ ৱর্ণ *s) {
        ৱর্ণ *r, *t;
        ধ্রুৱ ৱর্ণ *f;

        assert_return(s, রিক্ত);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        যদি (*s == 0)
                ঘূৰাই strdup("_");

        r = new(ৱর্ণ, strlen(s)*3 + 1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        বাবে (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                যদি (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } অন্যথা
                        *(t++) = *f;
        }

        *t = 0;

        ঘূৰাই r;
}

ৱর্ণ* bus_label_unescape_n(ধ্রুৱ ৱর্ণ *f, size_t l) {
        ৱর্ণ *r, *t;
        size_t i;

        assert_return(f, রিক্ত);

        যদি (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        যদি (l == 1 && *f == '_')
                ঘূৰাই strdup("");

        r = new(ৱর্ণ, l + 1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        বাবে (i = 0, t = r; i < l; ++i) {
                যদি (f[i] == '_') {
                        পূর্ণাঙ্ক a, b;

                        যদি (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } অন্যথা {
                                *(t++) = (ৱর্ণ) ((a << 4) | b);
                                i += 2;
                        }
                } অন্যথা
                        *(t++) = f[i];
        }

        *t = 0;

        ঘূৰাই r;
}
