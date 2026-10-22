/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "parse-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"

ثابت حرف* string_table_lookup_to_string(ثابت حرف * ثابت *table, size_t len, ssize_t i) {
        إذا (i < 0 || i >= (ssize_t) len)
                أرجع عدم;

        أكد(table);

        أرجع table[i];
}

ssize_t string_table_lookup_from_string(ثابت حرف * ثابت *table, size_t len, ثابت حرف *ئاچقۇچ) {
        assert_return(table, -EINVAL);

        إذا (!ئاچقۇچ)
                أرجع -EINVAL;

        لكل (size_t i = 0; i < len; ++i)
                إذا (streq_ptr(table[i], ئاچقۇچ))
                        أرجع (ssize_t) i;

        أرجع -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ثابت حرف * ثابت *table, size_t len, ثابت حرف *ئاچقۇچ, ssize_t yes) {
        إذا (!ئاچقۇچ)
                أرجع -EINVAL;

        صحيح b = parse_boolean(ئاچقۇچ);
        إذا (b == 0)
                أرجع 0;
        إذا (b > 0)
                أرجع yes;

        أرجع string_table_lookup_from_string(table, len, ئاچقۇچ);
}

صحيح string_table_lookup_to_string_fallback(ثابت حرف * ثابت *table, size_t len, ssize_t i, size_t max, حرف **ret) {
        حرف *s;

        أكد(table);
        أكد(ret);

        إذا (i < 0 || i > (ssize_t) max)
                أرجع -ERANGE;

        إذا (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        وإلا
                s = asprintf_safe("%zd", i);
        إذا (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

ssize_t string_table_lookup_from_string_fallback(ثابت حرف * ثابت *table, size_t len, ثابت حرف *s, size_t max) {
        إذا (!s)
                أرجع -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        إذا (i >= 0)
                أرجع i;

        unsigned u;
        إذا (safe_atou(s, &u) < 0)
                أرجع -EINVAL;
        إذا (u > max)
                أرجع -EINVAL;

        أرجع u;
}
