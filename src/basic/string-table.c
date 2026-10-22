/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ᬬᬾᬦ᭄ (i < 0 || i >= (ssize_t) len)
                ᬯᬮᬶ NULL;

        assert(table);

        ᬯᬮᬶ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ᬬᬾᬦ᭄ (!key)
                ᬯᬮᬶ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ᬬᬾᬦ᭄ (streq_ptr(table[i], key))
                        ᬯᬮᬶ (ssize_t) i;

        ᬯᬮᬶ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ᬬᬾᬦ᭄ (!key)
                ᬯᬮᬶ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ᬬᬾᬦ᭄ (b == 0)
                ᬯᬮᬶ 0;
        ᬬᬾᬦ᭄ (b > 0)
                ᬯᬮᬶ yes;

        ᬯᬮᬶ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ᬬᬾᬦ᭄ (i < 0 || i > (ssize_t) max)
                ᬯᬮᬶ -ERANGE;

        ᬬᬾᬦ᭄ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ᬮᬶᬬ
                s = asprintf_safe("%zd", i);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        *ret = s;
        ᬯᬮᬶ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ᬬᬾᬦ᭄ (i >= 0)
                ᬯᬮᬶ i;

        unsigned u;
        ᬬᬾᬦ᭄ (safe_atou(s, &u) < 0)
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (u > max)
                ᬯᬮᬶ -EINVAL;

        ᬯᬮᬶ u;
}
