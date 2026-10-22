/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "parse-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"

ثابت حرف* string_جدول_lookup_منزل_string(ثابت حرف * ثابت *جدول, size_t len, ssize_t i) {
        إذا (i < 0 || i >= (ssize_t) len)
                أرجع عدم;

        أكد(جدول);

        أرجع جدول[i];
}

ssize_t string_جدول_lookup_from_string(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *ڪنجي) {
        assert_return(جدول, -EINVAL);

        إذا (!ڪنجي)
                أرجع -EINVAL;

        لكل (size_t i = 0; i < len; ++i)
                إذا (streq_ptr(جدول[i], ڪنجي))
                        أرجع (ssize_t) i;

        أرجع -EINVAL;
}

ssize_t string_جدول_lookup_from_string_with_boolean(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *ڪنجي, ssize_t yes) {
        إذا (!ڪنجي)
                أرجع -EINVAL;

        صحيح b = parse_boolean(ڪنجي);
        إذا (b == 0)
                أرجع 0;
        إذا (b > 0)
                أرجع yes;

        أرجع string_جدول_lookup_from_string(جدول, len, ڪنجي);
}

صحيح string_جدول_lookup_منزل_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ssize_t i, size_t max, حرف **ret) {
        حرف *s;

        أكد(جدول);
        أكد(ret);

        إذا (i < 0 || i > (ssize_t) max)
                أرجع -ERANGE;

        إذا (i < (ssize_t) len && جدول[i])
                s = strdup(جدول[i]);
        وإلا
                s = asprintf_محفوظ("%zd", i);
        إذا (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

ssize_t string_جدول_lookup_from_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *s, size_t max) {
        إذا (!s)
                أرجع -EINVAL;

        ssize_t i = string_جدول_lookup_from_string(جدول, len, s);
        إذا (i >= 0)
                أرجع i;

        بنانشان u;
        إذا (محفوظ_atou(s, &u) < 0)
                أرجع -EINVAL;
        إذا (u > max)
                أرجع -EINVAL;

        أرجع u;
}
