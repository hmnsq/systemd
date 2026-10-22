/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ᨀᨘ (i < 0 || i >= (ssize_t) len)
                ᨀᨘᨒ NULL;

        assert(table);

        ᨀᨘᨒ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ᨀᨘ (!key)
                ᨀᨘᨒ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ᨀᨘ (streq_ptr(table[i], key))
                        ᨀᨘᨒ (ssize_t) i;

        ᨀᨘᨒ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ᨀᨘ (!key)
                ᨀᨘᨒ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ᨀᨘ (b == 0)
                ᨀᨘᨒ 0;
        ᨀᨘ (b > 0)
                ᨀᨘᨒ yes;

        ᨀᨘᨒ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ᨀᨘ (i < 0 || i > (ssize_t) max)
                ᨀᨘᨒ -ERANGE;

        ᨀᨘ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ᨀᨘᨁ
                s = asprintf_safe("%zd", i);
        ᨀᨘ (!s)
                ᨀᨘᨒ -ENOMEM;

        *ret = s;
        ᨀᨘᨒ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ᨀᨘ (!s)
                ᨀᨘᨒ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ᨀᨘ (i >= 0)
                ᨀᨘᨒ i;

        unsigned u;
        ᨀᨘ (safe_atou(s, &u) < 0)
                ᨀᨘᨒ -EINVAL;
        ᨀᨘ (u > max)
                ᨀᨘᨒ -EINVAL;

        ᨀᨘᨒ u;
}
