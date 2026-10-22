/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "bus-label.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "string-util.h"

അക്ഷരം* bus_label_escape(സ്ഥിരം അക്ഷരം *s) {
        അക്ഷരം *r, *t;
        സ്ഥിരം അക്ഷരം *f;

        assert_return(s, ശൂന്യ);

        /* Escapes all chars that D-Bus' object path cannot deal
         * with. Can be reversed with bus_path_unescape(). We special
         * case the empty string. */

        എങ്കിൽ (*s == 0)
                തിരികെ strdup("_");

        r = new(അക്ഷരം, strlen(s)*3 + 1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        വേണ്ടി (f = s, t = r; *f; f++) {

                /* Escape everything that is not a-zA-Z0-9. We also escape 0-9 if it's the first character */

                എങ്കിൽ (!ascii_isalpha(*f) &&
                    !(f > s && ascii_isdigit(*f))) {
                        *(t++) = '_';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } അല്ലെങ്കിൽ
                        *(t++) = *f;
        }

        *t = 0;

        തിരികെ r;
}

അക്ഷരം* bus_label_unescape_n(സ്ഥിരം അക്ഷരം *f, size_t l) {
        അക്ഷരം *r, *t;
        size_t i;

        assert_return(f, ശൂന്യ);

        എങ്കിൽ (l == SIZE_MAX)
                l = strlen(f);

        /* Special case for the empty string */
        എങ്കിൽ (l == 1 && *f == '_')
                തിരികെ strdup("");

        r = new(അക്ഷരം, l + 1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        വേണ്ടി (i = 0, t = r; i < l; ++i) {
                എങ്കിൽ (f[i] == '_') {
                        പൂർണ്ണസംഖ്യ a, b;

                        എങ്കിൽ (l - i < 3 ||
                            (a = unhexchar(f[i + 1])) < 0 ||
                            (b = unhexchar(f[i + 2])) < 0) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '_';
                        } അല്ലെങ്കിൽ {
                                *(t++) = (അക്ഷരം) ((a << 4) | b);
                                i += 2;
                        }
                } അല്ലെങ്കിൽ
                        *(t++) = f[i];
        }

        *t = 0;

        തിരികെ r;
}
