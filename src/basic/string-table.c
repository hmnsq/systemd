/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        གལ་སྲིད (i < 0 || i >= (ssize_t) len)
                ལོག NULL;

        assert(table);

        ལོག table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        གལ་སྲིད (!key)
                ལོག -EINVAL;

        དོན་ལུ (size_t i = 0; i < len; ++i)
                གལ་སྲིད (streq_ptr(table[i], key))
                        ལོག (ssize_t) i;

        ལོག -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        གལ་སྲིད (!key)
                ལོག -EINVAL;

        ཧྲིལ b = parse_boolean(key);
        གལ་སྲིད (b == 0)
                ལོག 0;
        གལ་སྲིད (b > 0)
                ལོག yes;

        ལོག string_table_lookup_from_string(table, len, key);
}

ཧྲིལ string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        གལ་སྲིད (i < 0 || i > (ssize_t) max)
                ལོག -ERANGE;

        གལ་སྲིད (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        དེ་མིན
                s = asprintf_safe("%zd", i);
        གལ་སྲིད (!s)
                ལོག -ENOMEM;

        *ret = s;
        ལོག 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        གལ་སྲིད (!s)
                ལོག -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        གལ་སྲིད (i >= 0)
                ལོག i;

        unsigned u;
        གལ་སྲིད (safe_atou(s, &u) < 0)
                ལོག -EINVAL;
        གལ་སྲིད (u > max)
                ལོག -EINVAL;

        ལོག u;
}
