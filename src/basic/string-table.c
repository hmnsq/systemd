/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        если (i < 0 || i >= (ssize_t) len)
                вернуть NULL;

        assert(table);

        вернуть table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        если (!key)
                вернуть -EINVAL;

        для (size_t i = 0; i < len; ++i)
                если (streq_ptr(table[i], key))
                        вернуть (ssize_t) i;

        вернуть -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        если (!key)
                вернуть -EINVAL;

        целое b = parse_boolean(key);
        если (b == 0)
                вернуть 0;
        если (b > 0)
                вернуть yes;

        вернуть string_table_lookup_from_string(table, len, key);
}

целое string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        если (i < 0 || i > (ssize_t) max)
                вернуть -ERANGE;

        если (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        иначе
                s = asprintf_safe("%zd", i);
        если (!s)
                вернуть -ENOMEM;

        *ret = s;
        вернуть 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        если (!s)
                вернуть -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        если (i >= 0)
                вернуть i;

        unsigned u;
        если (safe_atou(s, &u) < 0)
                вернуть -EINVAL;
        если (u > max)
                вернуть -EINVAL;

        вернуть u;
}
