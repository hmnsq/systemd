/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        якщо (i < 0 || i >= (ssize_t) len)
                повернути NULL;

        assert(table);

        повернути table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        якщо (!key)
                повернути -EINVAL;

        для (size_t i = 0; i < len; ++i)
                якщо (streq_ptr(table[i], key))
                        повернути (ssize_t) i;

        повернути -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        якщо (!key)
                повернути -EINVAL;

        ціле b = parse_boolean(key);
        якщо (b == 0)
                повернути 0;
        якщо (b > 0)
                повернути yes;

        повернути string_table_lookup_from_string(table, len, key);
}

ціле string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        якщо (i < 0 || i > (ssize_t) max)
                повернути -ERANGE;

        якщо (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        інакше
                s = asprintf_safe("%zd", i);
        якщо (!s)
                повернути -ENOMEM;

        *ret = s;
        повернути 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        якщо (!s)
                повернути -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        якщо (i >= 0)
                повернути i;

        unsigned u;
        якщо (safe_atou(s, &u) < 0)
                повернути -EINVAL;
        якщо (u > max)
                повернути -EINVAL;

        повернути u;
}
