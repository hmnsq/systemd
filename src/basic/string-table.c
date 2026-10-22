/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ᯀᯮᯝ (i < 0 || i >= (ssize_t) len)
                ᯀᯮᯞ NULL;

        assert(table);

        ᯀᯮᯞ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ᯀᯮᯝ (!key)
                ᯀᯮᯞ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ᯀᯮᯝ (streq_ptr(table[i], key))
                        ᯀᯮᯞ (ssize_t) i;

        ᯀᯮᯞ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ᯀᯮᯝ (!key)
                ᯀᯮᯞ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ᯀᯮᯝ (b == 0)
                ᯀᯮᯞ 0;
        ᯀᯮᯝ (b > 0)
                ᯀᯮᯞ yes;

        ᯀᯮᯞ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ᯀᯮᯝ (i < 0 || i > (ssize_t) max)
                ᯀᯮᯞ -ERANGE;

        ᯀᯮᯝ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ᯀᯮᯝᯂᯪ
                s = asprintf_safe("%zd", i);
        ᯀᯮᯝ (!s)
                ᯀᯮᯞ -ENOMEM;

        *ret = s;
        ᯀᯮᯞ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ᯀᯮᯝ (!s)
                ᯀᯮᯞ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ᯀᯮᯝ (i >= 0)
                ᯀᯮᯞ i;

        unsigned u;
        ᯀᯮᯝ (safe_atou(s, &u) < 0)
                ᯀᯮᯞ -EINVAL;
        ᯀᯮᯝ (u > max)
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯞ u;
}
