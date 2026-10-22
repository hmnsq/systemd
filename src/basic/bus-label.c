/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bus-label.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "string-util.h"

वर्ण* bus_label_escape(ध्रुव वर्ण *s) {
        वर्ण *r, *t;
        ध्रुव वर्ण *f;

        assert_return(s, रिक्त);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        सचे (*s == 0)
                पटिनिवत्त strdup("_");

        r = new(वर्ण, strlen(s)*3 + 1);
        सचे (!r)
                पटिनिवत्त रिक्त;

        अत्थाय (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                सचे (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अञ्ञथा
                        *(t++) = *f;
        }

        *t = 0;

        पटिनिवत्त r;
}

वर्ण* bus_label_unescape_n(ध्रुव वर्ण *f, size_t l) {
        वर्ण *r, *t;
        size_t i;

        assert_return(f, रिक्त);

        सचे (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        सचे (l == 1 && *f == '_')
                पटिनिवत्त strdup("");

        r = new(वर्ण, l + 1);
        सचे (!r)
                पटिनिवत्त रिक्त;

        अत्थाय (i = 0, t = r; i < l; ++i) {
                सचे (f[i] == '_') {
                        पूर्णाङ्क a, b;

                        सचे (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } अञ्ञथा {
                                *(t++) = (वर्ण) ((a << 4) | b);
                                i += 2;
                        }
                } अञ्ञथा
                        *(t++) = f[i];
        }

        *t = 0;

        पटिनिवत्त r;
}
