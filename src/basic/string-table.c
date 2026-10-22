/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ᤀᤡ᤺ (i < 0 || i >= (ssize_t) len)
                ᤀᤡᤴ NULL;

        assert(table);

        ᤀᤡᤴ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ᤀᤡ᤺ (!key)
                ᤀᤡᤴ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ᤀᤡ᤺ (streq_ptr(table[i], key))
                        ᤀᤡᤴ (ssize_t) i;

        ᤀᤡᤴ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ᤀᤡ᤺ (!key)
                ᤀᤡᤴ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ᤀᤡ᤺ (b == 0)
                ᤀᤡᤴ 0;
        ᤀᤡ᤺ (b > 0)
                ᤀᤡᤴ yes;

        ᤀᤡᤴ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ᤀᤡ᤺ (i < 0 || i > (ssize_t) max)
                ᤀᤡᤴ -ERANGE;

        ᤀᤡ᤺ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ᤀᤡ᤺ᤆᤡ
                s = asprintf_safe("%zd", i);
        ᤀᤡ᤺ (!s)
                ᤀᤡᤴ -ENOMEM;

        *ret = s;
        ᤀᤡᤴ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ᤀᤡ᤺ (!s)
                ᤀᤡᤴ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ᤀᤡ᤺ (i >= 0)
                ᤀᤡᤴ i;

        unsigned u;
        ᤀᤡ᤺ (safe_atou(s, &u) < 0)
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (u > max)
                ᤀᤡᤴ -EINVAL;

        ᤀᤡᤴ u;
}
