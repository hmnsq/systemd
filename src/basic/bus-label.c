/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "bus-label.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "string-util.h"

అక్షరం* bus_label_escape(స్థిరరాశి అక్షరం *s) {
        అక్షరం *r, *t;
        స్థిరరాశి అక్షరం *f;

        assert_return(s, ఖాళీ);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        ఒకవేళ (*s == 0)
                తిరిగి strdup("_");

        r = new(అక్షరం, అక్షరశ్రేణి_పొడవు(s)*3 + 1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        కొరకు (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                ఒకవేళ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } లేకపోతే
                        *(t++) = *f;
        }

        *t = 0;

        తిరిగి r;
}

అక్షరం* bus_label_unescape_n(స్థిరరాశి అక్షరం *f, size_t l) {
        అక్షరం *r, *t;
        size_t i;

        assert_return(f, ఖాళీ);

        ఒకవేళ (l == SIZE_MAX)
                l = అక్షరశ్రేణి_పొడవు(f);

        /* Special case for the empty string */
        ఒకవేళ (l == 1 && *f == '_')
                తిరిగి strdup("");

        r = new(అక్షరం, l + 1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        కొరకు (i = 0, t = r; i < l; ++i) {
                ఒకవేళ (f[i] == '_') {
                        పూర్ణాంకం a, b;

                        ఒకవేళ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } లేకపోతే {
                                *(t++) = (అక్షరం) ((a << 4) | b);
                                i += 2;
                        }
                } లేకపోతే
                        *(t++) = f[i];
        }

        *t = 0;

        తిరిగి r;
}
