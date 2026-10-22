/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "parse-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"

ثابت حرف* string_table_lookup_to_string(ثابت حرف * ثابت *table, size_t len, ssize_t i) {
        𐤀𐤌 (i < 0 || i >= (ssize_t) len)
                أرجع عدم;

        أكد(table);

        أرجع table[i];
}

ssize_t string_table_lookup_from_string(ثابت حرف * ثابت *table, size_t len, ثابت حرف *key) {
        assert_return(table, -EINVAL);

        𐤀𐤌 (!key)
                أرجع -EINVAL;

        لكل (size_t i = 0; i < len; ++i)
                𐤀𐤌 (streq_ptr(table[i], key))
                        أرجع (ssize_t) i;

        أرجع -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ثابت حرف * ثابت *table, size_t len, ثابت حرف *key, ssize_t yes) {
        𐤀𐤌 (!key)
                أرجع -EINVAL;

        صحيح b = parse_boolean(key);
        𐤀𐤌 (b == 0)
                أرجع 0;
        𐤀𐤌 (b > 0)
                أرجع yes;

        أرجع string_table_lookup_from_string(table, len, key);
}

صحيح string_table_lookup_to_string_fallback(ثابت حرف * ثابت *table, size_t len, ssize_t i, size_t max, حرف **ret) {
        حرف *s;

        أكد(table);
        أكد(ret);

        𐤀𐤌 (i < 0 || i > (ssize_t) max)
                أرجع -ERANGE;

        𐤀𐤌 (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        وإلا
                s = asprintf_safe("%zd", i);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

ssize_t string_table_lookup_from_string_fallback(ثابت حرف * ثابت *table, size_t len, ثابت حرف *s, size_t max) {
        𐤀𐤌 (!s)
                أرجع -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        𐤀𐤌 (i >= 0)
                أرجع i;

        unsigned u;
        𐤀𐤌 (safe_atou(s, &u) < 0)
                أرجع -EINVAL;
        𐤀𐤌 (u > max)
                أرجع -EINVAL;

        أرجع u;
}
