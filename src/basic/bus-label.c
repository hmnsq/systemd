/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bus-label.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "string-util.h"

वर्ण* bus_label_escape(अचर वर्ण *s) {
        वर्ण *r, *t;
        अचर वर्ण *f;

        assert_return(s, रिक्त);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        यदि (*s == 0)
                फर्काऊ strdup("_");

        r = new(वर्ण, म_माप(s)*3 + 1);
        यदि (!r)
                फर्काऊ रिक्त;

        लागि (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                यदि (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अन्यथा
                        *(t++) = *f;
        }

        *t = 0;

        फर्काऊ r;
}

वर्ण* bus_label_unescape_n(अचर वर्ण *f, माप_प्रकार l) {
        वर्ण *r, *t;
        माप_प्रकार i;

        assert_return(f, रिक्त);

        यदि (l == SIZE_MAX)
                l = म_माप(f);

        /* Special case for the empty string */
        यदि (l == 1 && *f == '_')
                फर्काऊ strdup("");

        r = new(वर्ण, l + 1);
        यदि (!r)
                फर्काऊ रिक्त;

        लागि (i = 0, t = r; i < l; ++i) {
                यदि (f[i] == '_') {
                        पूर्णाङ्क a, b;

                        यदि (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } अन्यथा {
                                *(t++) = (वर्ण) ((a << 4) | b);
                                i += 2;
                        }
                } अन्यथा
                        *(t++) = f[i];
        }

        *t = 0;

        फर्काऊ r;
}
