/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ပေႃး (i < 0 || i >= (ssize_t) len)
                ပွၵ်ႈ NULL;

        assert(table);

        ပွၵ်ႈ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ပေႃး (!key)
                ပွၵ်ႈ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ပေႃး (streq_ptr(table[i], key))
                        ပွၵ်ႈ (ssize_t) i;

        ပွၵ်ႈ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ပေႃး (!key)
                ပွၵ်ႈ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ပေႃး (b == 0)
                ပွၵ်ႈ 0;
        ပေႃး (b > 0)
                ပွၵ်ႈ yes;

        ပွၵ်ႈ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ပေႃး (i < 0 || i > (ssize_t) max)
                ပွၵ်ႈ -ERANGE;

        ပေႃး (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ပေႃးဢမ်ႇ
                s = asprintf_safe("%zd", i);
        ပေႃး (!s)
                ပွၵ်ႈ -ENOMEM;

        *ret = s;
        ပွၵ်ႈ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ပေႃး (!s)
                ပွၵ်ႈ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ပေႃး (i >= 0)
                ပွၵ်ႈ i;

        unsigned u;
        ပေႃး (safe_atou(s, &u) < 0)
                ပွၵ်ႈ -EINVAL;
        ပေႃး (u > max)
                ပွၵ်ႈ -EINVAL;

        ပွၵ်ႈ u;
}
