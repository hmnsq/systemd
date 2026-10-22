/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        ако (i < 0 || i >= (ssize_t) len)
                върни NULL;

        assert(table);

        върни table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        ако (!key)
                върни -EINVAL;

        за (size_t i = 0; i < len; ++i)
                ако (streq_ptr(table[i], key))
                        върни (ssize_t) i;

        върни -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        ако (!key)
                върни -EINVAL;

        цяло b = parse_boolean(key);
        ако (b == 0)
                върни 0;
        ако (b > 0)
                върни yes;

        върни string_table_lookup_from_string(table, len, key);
}

цяло string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        ако (i < 0 || i > (ssize_t) max)
                върни -ERANGE;

        ако (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        иначе
                s = asprintf_safe("%zd", i);
        ако (!s)
                върни -ENOMEM;

        *ret = s;
        върни 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        ако (!s)
                върни -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ако (i >= 0)
                върни i;

        unsigned u;
        ако (safe_atou(s, &u) < 0)
                върни -EINVAL;
        ако (u > max)
                върни -EINVAL;

        върни u;
}
