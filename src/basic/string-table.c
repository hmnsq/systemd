/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಧ್ರುವ ವರ್ಣ* string_table_lookup_to_string(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *table, size_t len, ssize_t i) {
        ಯದಿ (i < 0 || i >= (ssize_t) len)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(table);

        ಪ್ರತ್ಯಾಗಮ table[i];
}

ssize_t string_table_lookup_from_string(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *table, size_t len, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ) {
        assert_return(table, -EINVAL);

        ಯದಿ (!ಕೀಲಿ)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತಿ (size_t i = 0; i < len; ++i)
                ಯದಿ (streq_ptr(table[i], ಕೀಲಿ))
                        ಪ್ರತ್ಯಾಗಮ (ssize_t) i;

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *table, size_t len, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ssize_t yes) {
        ಯದಿ (!ಕೀಲಿ)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪೂರ್ಣಾಙ್ಕ b = parse_boolean(ಕೀಲಿ);
        ಯದಿ (b == 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (b > 0)
                ಪ್ರತ್ಯಾಗಮ yes;

        ಪ್ರತ್ಯಾಗಮ string_table_lookup_from_string(table, len, ಕೀಲಿ);
}

ಪೂರ್ಣಾಙ್ಕ string_table_lookup_to_string_fallback(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *table, size_t len, ssize_t i, size_t max, ವರ್ಣ **ret) {
        ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(table);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (i < 0 || i > (ssize_t) max)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ಅನ್ಯಥಾ
                s = asprintf_safe("%zd", i);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = s;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ssize_t string_table_lookup_from_string_fallback(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *table, size_t len, ಧ್ರುವ ವರ್ಣ *s, size_t max) {
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ಯದಿ (i >= 0)
                ಪ್ರತ್ಯಾಗಮ i;

        unsigned u;
        ಯದಿ (safe_atou(s, &u) < 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (u > max)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ u;
}
