/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "parse-util.h"
#شامل "stdio-util.h"
#شامل "string-table.h"
#شامل "string-util.h"

ثابت حرف* string_جدول_lookup_مقصد_string(ثابت حرف * ثابت *جدول, size_t len, ssize_t i) {
        اگر (i < 0 || i >= (ssize_t) len)
                واپس تہی;

        أكد(جدول);

        واپس جدول[i];
}

ssize_t string_جدول_lookup_from_string(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *کلید) {
        assert_return(جدول, -EINVAL);

        اگر (!کلید)
                واپس -EINVAL;

        برائے (size_t i = 0; i < len; ++i)
                اگر (streq_ptr(جدول[i], کلید))
                        واپس (ssize_t) i;

        واپس -EINVAL;
}

ssize_t string_جدول_lookup_from_string_with_boolean(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *کلید, ssize_t yes) {
        اگر (!کلید)
                واپس -EINVAL;

        صحیح b = parse_boolean(کلید);
        اگر (b == 0)
                واپس 0;
        اگر (b > 0)
                واپس yes;

        واپس string_جدول_lookup_from_string(جدول, len, کلید);
}

صحیح string_جدول_lookup_مقصد_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ssize_t i, size_t max, حرف **ret) {
        حرف *s;

        أكد(جدول);
        أكد(ret);

        اگر (i < 0 || i > (ssize_t) max)
                واپس -ERANGE;

        اگر (i < (ssize_t) len && جدول[i])
                s = strdup(جدول[i]);
        ورنہ
                s = asprintf_محفوظ("%zd", i);
        اگر (!s)
                واپس -ENOMEM;

        *ret = s;
        واپس 0;
}

ssize_t string_جدول_lookup_from_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *s, size_t max) {
        اگر (!s)
                واپس -EINVAL;

        ssize_t i = string_جدول_lookup_from_string(جدول, len, s);
        اگر (i >= 0)
                واپس i;

        غیرعلامتی u;
        اگر (محفوظ_atou(s, &u) < 0)
                واپس -EINVAL;
        اگر (u > max)
                واپس -EINVAL;

        واپس u;
}
