/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-table.h"
#অন্তর্ভাৱ "string-util.h"

ধ্রুৱ ৱর্ণ* string_table_lookup_to_string(ধ্রুৱ ৱর্ণ * ধ্রুৱ *table, size_t len, ssize_t i) {
        যদি (i < 0 || i >= (ssize_t) len)
                ঘূৰাই রিক্ত;

        প্রতিজ্ঞা(table);

        ঘূৰাই table[i];
}

ssize_t string_table_lookup_from_string(ধ্রুৱ ৱর্ণ * ধ্রুৱ *table, size_t len, ধ্রুৱ ৱর্ণ *চাবি) {
        assert_return(table, -EINVAL);

        যদি (!চাবি)
                ঘূৰাই -EINVAL;

        বাবে (size_t i = 0; i < len; ++i)
                যদি (streq_ptr(table[i], চাবি))
                        ঘূৰাই (ssize_t) i;

        ঘূৰাই -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ধ্রুৱ ৱর্ণ * ধ্রুৱ *table, size_t len, ধ্রুৱ ৱর্ণ *চাবি, ssize_t yes) {
        যদি (!চাবি)
                ঘূৰাই -EINVAL;

        পূর্ণাঙ্ক b = parse_boolean(চাবি);
        যদি (b == 0)
                ঘূৰাই 0;
        যদি (b > 0)
                ঘূৰাই yes;

        ঘূৰাই string_table_lookup_from_string(table, len, চাবি);
}

পূর্ণাঙ্ক string_table_lookup_to_string_fallback(ধ্রুৱ ৱর্ণ * ধ্রুৱ *table, size_t len, ssize_t i, size_t max, ৱর্ণ **ret) {
        ৱর্ণ *s;

        প্রতিজ্ঞা(table);
        প্রতিজ্ঞা(ret);

        যদি (i < 0 || i > (ssize_t) max)
                ঘূৰাই -ERANGE;

        যদি (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        অন্যথা
                s = asprintf_safe("%zd", i);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        *ret = s;
        ঘূৰাই 0;
}

ssize_t string_table_lookup_from_string_fallback(ধ্রুৱ ৱর্ণ * ধ্রুৱ *table, size_t len, ধ্রুৱ ৱর্ণ *s, size_t max) {
        যদি (!s)
                ঘূৰাই -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        যদি (i >= 0)
                ঘূৰাই i;

        unsigned u;
        যদি (safe_atou(s, &u) < 0)
                ঘূৰাই -EINVAL;
        যদি (u > max)
                ঘূৰাই -EINVAL;

        ঘূৰাই u;
}
