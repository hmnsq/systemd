/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const char* string_table_lookup_to_string(const char * const *table, size_t len, ssize_t i) {
        nếu (i < 0 || i >= (ssize_t) len)
                trả_về NULL;

        assert(table);

        trả_về table[i];
}

ssize_t string_table_lookup_from_string(const char * const *table, size_t len, const char *key) {
        assert_return(table, -EINVAL);

        nếu (!key)
                trả_về -EINVAL;

        cho (size_t i = 0; i < len; ++i)
                nếu (streq_ptr(table[i], key))
                        trả_về (ssize_t) i;

        trả_về -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const char * const *table, size_t len, const char *key, ssize_t yes) {
        nếu (!key)
                trả_về -EINVAL;

        số_nguyên b = parse_boolean(key);
        nếu (b == 0)
                trả_về 0;
        nếu (b > 0)
                trả_về yes;

        trả_về string_table_lookup_from_string(table, len, key);
}

số_nguyên string_table_lookup_to_string_fallback(const char * const *table, size_t len, ssize_t i, size_t max, char **ret) {
        char *s;

        assert(table);
        assert(ret);

        nếu (i < 0 || i > (ssize_t) max)
                trả_về -ERANGE;

        nếu (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        không_thì
                s = asprintf_safe("%zd", i);
        nếu (!s)
                trả_về -ENOMEM;

        *ret = s;
        trả_về 0;
}

ssize_t string_table_lookup_from_string_fallback(const char * const *table, size_t len, const char *s, size_t max) {
        nếu (!s)
                trả_về -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        nếu (i >= 0)
                trả_về i;

        unsigned u;
        nếu (safe_atou(s, &u) < 0)
                trả_về -EINVAL;
        nếu (u > max)
                trả_về -EINVAL;

        trả_về u;
}
