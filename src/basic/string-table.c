/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        གལ་ཏེ (i < 0 || i >= (ssize_t) len)
                ཕྱིར NULL;

        assert(table);

        ཕྱིར table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        གལ་ཏེ (!key)
                ཕྱིར -EINVAL;

        ཆེད་དུ (size_t i = 0; i < len; ++i)
                གལ་ཏེ (streq_ptr(table[i], key))
                        ཕྱིར (ssize_t) i;

        ཕྱིར -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        གལ་ཏེ (!key)
                ཕྱིར -EINVAL;

        ཧྲིལ b = parse_boolean(key);
        གལ་ཏེ (b == 0)
                ཕྱིར 0;
        གལ་ཏེ (b > 0)
                ཕྱིར yes;

        ཕྱིར string_table_lookup_from_string(table, len, key);
}

ཧྲིལ string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        གལ་ཏེ (i < 0 || i > (ssize_t) max)
                ཕྱིར -ERANGE;

        གལ་ཏེ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        དེ་མིན
                s = asprintf_safe("%zd", i);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        *ret = s;
        ཕྱིར 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        གལ་ཏེ (!s)
                ཕྱིར -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        གལ་ཏེ (i >= 0)
                ཕྱིར i;

        unsigned u;
        གལ་ཏེ (safe_atou(s, &u) < 0)
                ཕྱིར -EINVAL;
        གལ་ཏེ (u > max)
                ཕྱིར -EINVAL;

        ཕྱིར u;
}
