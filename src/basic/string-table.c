/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"

സ്ഥിരം അക്ഷരം* string_table_lookup_to_string(സ്ഥിരം അക്ഷരം * സ്ഥിരം *table, size_t len, ssize_t i) {
        എങ്കിൽ (i < 0 || i >= (ssize_t) len)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(table);

        തിരികെ table[i];
}

ssize_t string_table_lookup_from_string(സ്ഥിരം അക്ഷരം * സ്ഥിരം *table, size_t len, സ്ഥിരം അക്ഷരം *താക്കോൽ) {
        assert_return(table, -EINVAL);

        എങ്കിൽ (!താക്കോൽ)
                തിരികെ -EINVAL;

        വേണ്ടി (size_t i = 0; i < len; ++i)
                എങ്കിൽ (streq_ptr(table[i], താക്കോൽ))
                        തിരികെ (ssize_t) i;

        തിരികെ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(സ്ഥിരം അക്ഷരം * സ്ഥിരം *table, size_t len, സ്ഥിരം അക്ഷരം *താക്കോൽ, ssize_t yes) {
        എങ്കിൽ (!താക്കോൽ)
                തിരികെ -EINVAL;

        പൂർണ്ണസംഖ്യ b = parse_boolean(താക്കോൽ);
        എങ്കിൽ (b == 0)
                തിരികെ 0;
        എങ്കിൽ (b > 0)
                തിരികെ yes;

        തിരികെ string_table_lookup_from_string(table, len, താക്കോൽ);
}

പൂർണ്ണസംഖ്യ string_table_lookup_to_string_fallback(സ്ഥിരം അക്ഷരം * സ്ഥിരം *table, size_t len, ssize_t i, size_t max, അക്ഷരം **ret) {
        അക്ഷരം *s;

        പ്രതിജ്ഞാ(table);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (i < 0 || i > (ssize_t) max)
                തിരികെ -ERANGE;

        എങ്കിൽ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        അല്ലെങ്കിൽ
                s = asprintf_safe("%zd", i);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        *ret = s;
        തിരികെ 0;
}

ssize_t string_table_lookup_from_string_fallback(സ്ഥിരം അക്ഷരം * സ്ഥിരം *table, size_t len, സ്ഥിരം അക്ഷരം *s, size_t max) {
        എങ്കിൽ (!s)
                തിരികെ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        എങ്കിൽ (i >= 0)
                തിരികെ i;

        unsigned u;
        എങ്കിൽ (safe_atou(s, &u) < 0)
                തിരികെ -EINVAL;
        എങ്കിൽ (u > max)
                തിരികെ -EINVAL;

        തിരികെ u;
}
