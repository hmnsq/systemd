/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ຖ້າ (i < 0 || i >= (ssize_t) len)
                ສົ່ງຄືນ NULL;

        assert(table);

        ສົ່ງຄືນ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ຖ້າ (!key)
                ສົ່ງຄືນ -EINVAL;

        ສຳລັບ (size_t i = 0; i < len; ++i)
                ຖ້າ (streq_ptr(table[i], key))
                        ສົ່ງຄືນ (ssize_t) i;

        ສົ່ງຄືນ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ຖ້າ (!key)
                ສົ່ງຄືນ -EINVAL;

        ຈຳນວນເຕັມ b = parse_boolean(key);
        ຖ້າ (b == 0)
                ສົ່ງຄືນ 0;
        ຖ້າ (b > 0)
                ສົ່ງຄືນ yes;

        ສົ່ງຄືນ string_table_lookup_from_string(table, len, key);
}

ຈຳນວນເຕັມ string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ຖ້າ (i < 0 || i > (ssize_t) max)
                ສົ່ງຄືນ -ERANGE;

        ຖ້າ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ບໍ່ດັ່ງນັ້ນ
                s = asprintf_safe("%zd", i);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        *ret = s;
        ສົ່ງຄືນ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ຖ້າ (!s)
                ສົ່ງຄືນ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ຖ້າ (i >= 0)
                ສົ່ງຄືນ i;

        unsigned u;
        ຖ້າ (safe_atou(s, &u) < 0)
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (u > max)
                ສົ່ງຄືນ -EINVAL;

        ສົ່ງຄືນ u;
}
