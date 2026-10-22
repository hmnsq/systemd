/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        အကယ်၍ (i < 0 || i >= (ssize_t) len)
                ပြန်ပေး NULL;

        assert(table);

        ပြန်ပေး table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        အကယ်၍ (!key)
                ပြန်ပေး -EINVAL;

        အတွက် (size_t i = 0; i < len; ++i)
                အကယ်၍ (streq_ptr(table[i], key))
                        ပြန်ပေး (ssize_t) i;

        ပြန်ပေး -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        အကယ်၍ (!key)
                ပြန်ပေး -EINVAL;

        ကိန်းပြည့် b = parse_boolean(key);
        အကယ်၍ (b == 0)
                ပြန်ပေး 0;
        အကယ်၍ (b > 0)
                ပြန်ပေး yes;

        ပြန်ပေး string_table_lookup_from_string(table, len, key);
}

ကိန်းပြည့် string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        အကယ်၍ (i < 0 || i > (ssize_t) max)
                ပြန်ပေး -ERANGE;

        အကယ်၍ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        မဟုတ်ပါက
                s = asprintf_safe("%zd", i);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        *ret = s;
        ပြန်ပေး 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        အကယ်၍ (!s)
                ပြန်ပေး -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        အကယ်၍ (i >= 0)
                ပြန်ပေး i;

        unsigned u;
        အကယ်၍ (safe_atou(s, &u) < 0)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (u > max)
                ပြန်ပေး -EINVAL;

        ပြန်ပေး u;
}
