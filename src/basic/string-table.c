/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ถ้า (i < 0 || i >= (ssize_t) len)
                คืน NULL;

        assert(table);

        คืน table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ถ้า (!key)
                คืน -EINVAL;

        สำหรับ (size_t i = 0; i < len; ++i)
                ถ้า (streq_ptr(table[i], key))
                        คืน (ssize_t) i;

        คืน -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ถ้า (!key)
                คืน -EINVAL;

        จำนวนเต็ม b = parse_boolean(key);
        ถ้า (b == 0)
                คืน 0;
        ถ้า (b > 0)
                คืน yes;

        คืน string_table_lookup_from_string(table, len, key);
}

จำนวนเต็ม string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ถ้า (i < 0 || i > (ssize_t) max)
                คืน -ERANGE;

        ถ้า (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        มิฉะนั้น
                s = asprintf_safe("%zd", i);
        ถ้า (!s)
                คืน -ENOMEM;

        *ret = s;
        คืน 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ถ้า (!s)
                คืน -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ถ้า (i >= 0)
                คืน i;

        unsigned u;
        ถ้า (safe_atou(s, &u) < 0)
                คืน -EINVAL;
        ถ้า (u > max)
                คืน -EINVAL;

        คืน u;
}
