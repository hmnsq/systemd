/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        如果 (i < 0 || i >= (ssize_t) len)
                返回 NULL;

        assert(table);

        返回 table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        如果 (!key)
                返回 -EINVAL;

        为 (size_t i = 0; i < len; ++i)
                如果 (streq_ptr(table[i], key))
                        返回 (ssize_t) i;

        返回 -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        如果 (!key)
                返回 -EINVAL;

        整数 b = parse_boolean(key);
        如果 (b == 0)
                返回 0;
        如果 (b > 0)
                返回 yes;

        返回 string_table_lookup_from_string(table, len, key);
}

整数 string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        如果 (i < 0 || i > (ssize_t) max)
                返回 -ERANGE;

        如果 (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        否则
                s = asprintf_safe("%zd", i);
        如果 (!s)
                返回 -ENOMEM;

        *ret = s;
        返回 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        如果 (!s)
                返回 -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        如果 (i >= 0)
                返回 i;

        unsigned u;
        如果 (safe_atou(s, &u) < 0)
                返回 -EINVAL;
        如果 (u > max)
                返回 -EINVAL;

        返回 u;
}
