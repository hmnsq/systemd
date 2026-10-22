/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "parse-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"

ثابت حرف* string_table_lookup_to_string(ثابت حرف * ثابت *table, size_t len, ssize_t i) {
        إذا (i < 0 || i >= (ssize_t) len)
                إرجاع عدم;

        أكد(table);

        إرجاع table[i];
}

ssize_t string_table_lookup_from_string(ثابت حرف * ثابت *table, size_t len, ثابت حرف *مفتاح) {
        assert_return(table, -EINVAL);

        إذا (!مفتاح)
                إرجاع -EINVAL;

        لكل (size_t i = 0; i < len; ++i)
                إذا (streq_ptr(table[i], مفتاح))
                        إرجاع (ssize_t) i;

        إرجاع -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ثابت حرف * ثابت *table, size_t len, ثابت حرف *مفتاح, ssize_t yes) {
        إذا (!مفتاح)
                إرجاع -EINVAL;

        صحيح b = parse_boolean(مفتاح);
        إذا (b == 0)
                إرجاع 0;
        إذا (b > 0)
                إرجاع yes;

        إرجاع string_table_lookup_from_string(table, len, مفتاح);
}

صحيح string_table_lookup_to_string_fallback(ثابت حرف * ثابت *table, size_t len, ssize_t i, size_t max, حرف **ret) {
        حرف *s;

        أكد(table);
        أكد(ret);

        إذا (i < 0 || i > (ssize_t) max)
                إرجاع -ERANGE;

        إذا (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        وإلا
                s = asprintf_safe("%zd", i);
        إذا (!s)
                إرجاع -ENOMEM;

        *ret = s;
        إرجاع 0;
}

ssize_t string_table_lookup_from_string_fallback(ثابت حرف * ثابت *table, size_t len, ثابت حرف *s, size_t max) {
        إذا (!s)
                إرجاع -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        إذا (i >= 0)
                إرجاع i;

        unsigned u;
        إذا (safe_atou(s, &u) < 0)
                إرجاع -EINVAL;
        إذا (u > max)
                إرجاع -EINVAL;

        إرجاع u;
}
