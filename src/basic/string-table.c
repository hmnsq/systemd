/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ᱡᱩᱫᱤ (i < 0 || i >= (ssize_t) len)
                ᱨᱩᱣᱟᱹᱲ NULL;

        assert(table);

        ᱨᱩᱣᱟᱹᱲ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ᱡᱩᱫᱤ (!key)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ᱡᱩᱫᱤ (streq_ptr(table[i], key))
                        ᱨᱩᱣᱟᱹᱲ (ssize_t) i;

        ᱨᱩᱣᱟᱹᱲ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ᱡᱩᱫᱤ (!key)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ᱡᱩᱫᱤ (b == 0)
                ᱨᱩᱣᱟᱹᱲ 0;
        ᱡᱩᱫᱤ (b > 0)
                ᱨᱩᱣᱟᱹᱲ yes;

        ᱨᱩᱣᱟᱹᱲ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ᱡᱩᱫᱤ (i < 0 || i > (ssize_t) max)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        ᱡᱩᱫᱤ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ᱵᱟᱝ
                s = asprintf_safe("%zd", i);
        ᱡᱩᱫᱤ (!s)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        *ret = s;
        ᱨᱩᱣᱟᱹᱲ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ᱡᱩᱫᱤ (!s)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ᱡᱩᱫᱤ (i >= 0)
                ᱨᱩᱣᱟᱹᱲ i;

        unsigned u;
        ᱡᱩᱫᱤ (safe_atou(s, &u) < 0)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;
        ᱡᱩᱫᱤ (u > max)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱨᱩᱣᱟᱹᱲ u;
}
