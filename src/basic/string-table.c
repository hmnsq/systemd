/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "parse-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"

ثابت حرف* string_table_lookup_to_string(ثابت حرف * ثابت *table, size_t len, ssize_t i) {
        ܐܢ (i < 0 || i >= (ssize_t) len)
                ܦܢܐ عدم;

        أكد(table);

        ܦܢܐ table[i];
}

ssize_t string_table_lookup_from_string(ثابت حرف * ثابت *table, size_t len, ثابت حرف *key) {
        assert_return(table, -EINVAL);

        ܐܢ (!key)
                ܦܢܐ -EINVAL;

        لكل (size_t i = 0; i < len; ++i)
                ܐܢ (streq_ptr(table[i], key))
                        ܦܢܐ (ssize_t) i;

        ܦܢܐ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ثابت حرف * ثابت *table, size_t len, ثابت حرف *key, ssize_t yes) {
        ܐܢ (!key)
                ܦܢܐ -EINVAL;

        صحيح b = parse_boolean(key);
        ܐܢ (b == 0)
                ܦܢܐ 0;
        ܐܢ (b > 0)
                ܦܢܐ yes;

        ܦܢܐ string_table_lookup_from_string(table, len, key);
}

صحيح string_table_lookup_to_string_fallback(ثابت حرف * ثابت *table, size_t len, ssize_t i, size_t max, حرف **ret) {
        حرف *s;

        أكد(table);
        أكد(ret);

        ܐܢ (i < 0 || i > (ssize_t) max)
                ܦܢܐ -ERANGE;

        ܐܢ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ܐܚܪܢܐ
                s = asprintf_safe("%zd", i);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        *ret = s;
        ܦܢܐ 0;
}

ssize_t string_table_lookup_from_string_fallback(ثابت حرف * ثابت *table, size_t len, ثابت حرف *s, size_t max) {
        ܐܢ (!s)
                ܦܢܐ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ܐܢ (i >= 0)
                ܦܢܐ i;

        unsigned u;
        ܐܢ (safe_atou(s, &u) < 0)
                ܦܢܐ -EINVAL;
        ܐܢ (u > max)
                ܦܢܐ -EINVAL;

        ܦܢܐ u;
}
