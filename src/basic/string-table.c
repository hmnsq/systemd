/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        もし (i < 0 || i >= (ssize_t) len)
                戻る NULL;

        assert(table);

        戻る table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        もし (!key)
                戻る -EINVAL;

        ために (size_t i = 0; i < len; ++i)
                もし (streq_ptr(table[i], key))
                        戻る (ssize_t) i;

        戻る -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        もし (!key)
                戻る -EINVAL;

        整数 b = parse_boolean(key);
        もし (b == 0)
                戻る 0;
        もし (b > 0)
                戻る yes;

        戻る string_table_lookup_from_string(table, len, key);
}

整数 string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        もし (i < 0 || i > (ssize_t) max)
                戻る -ERANGE;

        もし (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        否则
                s = asprintf_safe("%zd", i);
        もし (!s)
                戻る -ENOMEM;

        *ret = s;
        戻る 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        もし (!s)
                戻る -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        もし (i >= 0)
                戻る i;

        unsigned u;
        もし (safe_atou(s, &u) < 0)
                戻る -EINVAL;
        もし (u > max)
                戻る -EINVAL;

        戻る u;
}
