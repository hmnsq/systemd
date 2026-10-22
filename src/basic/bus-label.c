/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>

#समावेश "alloc-util.h"
#समावेश "bus-label.h"
#समावेश "hexdecoct.h"
#समावेश "string-util.h"

अक्षर* bus_label_escape(स्थिर अक्षर *s) {
        अक्षर *r, *t;
        स्थिर अक्षर *f;

        assert_return(s, शून्य);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        अगर (*s == 0)
                वापस strdup("_");

        r = new(अक्षर, म_माप(s)*3 + 1);
        अगर (!r)
                वापस शून्य;

        क्रम (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                अगर (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अन्यथा
                        *(t++) = *f;
        }

        *t = 0;

        वापस r;
}

अक्षर* bus_label_unescape_n(स्थिर अक्षर *f, माप_प्रकार l) {
        अक्षर *r, *t;
        माप_प्रकार i;

        assert_return(f, शून्य);

        अगर (l == SIZE_MAX)
                l = म_माप(f);

        /* Special case for the empty string */
        अगर (l == 1 && *f == '_')
                वापस strdup("");

        r = new(अक्षर, l + 1);
        अगर (!r)
                वापस शून्य;

        क्रम (i = 0, t = r; i < l; ++i) {
                अगर (f[i] == '_') {
                        पूर्णांक a, b;

                        अगर (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } अन्यथा {
                                *(t++) = (अक्षर) ((a << 4) | b);
                                i += 2;
                        }
                } अन्यथा
                        *(t++) = f[i];
        }

        *t = 0;

        वापस r;
}
