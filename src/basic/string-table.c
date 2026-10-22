/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        만약 (i < 0 || i >= (ssize_t) len)
                반환 NULL;

        assert(table);

        반환 table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        만약 (!key)
                반환 -EINVAL;

        위해 (size_t i = 0; i < len; ++i)
                만약 (streq_ptr(table[i], key))
                        반환 (ssize_t) i;

        반환 -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        만약 (!key)
                반환 -EINVAL;

        정수 b = parse_boolean(key);
        만약 (b == 0)
                반환 0;
        만약 (b > 0)
                반환 yes;

        반환 string_table_lookup_from_string(table, len, key);
}

정수 string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        만약 (i < 0 || i > (ssize_t) max)
                반환 -ERANGE;

        만약 (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        아니면
                s = asprintf_safe("%zd", i);
        만약 (!s)
                반환 -ENOMEM;

        *ret = s;
        반환 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        만약 (!s)
                반환 -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        만약 (i >= 0)
                반환 i;

        unsigned u;
        만약 (safe_atou(s, &u) < 0)
                반환 -EINVAL;
        만약 (u > max)
                반환 -EINVAL;

        반환 u;
}
