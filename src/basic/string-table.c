/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"

ধ্রুবক অক্ষর* string_table_lookup_to_string(ধ্রুবক অক্ষর * ধ্রুবক *table, size_t len, ssize_t i) {
        যদি (i < 0 || i >= (ssize_t) len)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(table);

        ফেরত table[i];
}

ssize_t string_table_lookup_from_string(ধ্রুবক অক্ষর * ধ্রুবক *table, size_t len, ধ্রুবক অক্ষর *চাবি) {
        assert_return(table, -EINVAL);

        যদি (!চাবি)
                ফেরত -EINVAL;

        জন্য (size_t i = 0; i < len; ++i)
                যদি (streq_ptr(table[i], চাবি))
                        ফেরত (ssize_t) i;

        ফেরত -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ধ্রুবক অক্ষর * ধ্রুবক *table, size_t len, ধ্রুবক অক্ষর *চাবি, ssize_t yes) {
        যদি (!চাবি)
                ফেরত -EINVAL;

        পূর্ণসংখ্যা b = parse_boolean(চাবি);
        যদি (b == 0)
                ফেরত 0;
        যদি (b > 0)
                ফেরত yes;

        ফেরত string_table_lookup_from_string(table, len, চাবি);
}

পূর্ণসংখ্যা string_table_lookup_to_string_fallback(ধ্রুবক অক্ষর * ধ্রুবক *table, size_t len, ssize_t i, size_t max, অক্ষর **ret) {
        অক্ষর *s;

        প্রতিজ্ঞা(table);
        প্রতিজ্ঞা(ret);

        যদি (i < 0 || i > (ssize_t) max)
                ফেরত -ERANGE;

        যদি (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        অন্যথা
                s = asprintf_safe("%zd", i);
        যদি (!s)
                ফেরত -ENOMEM;

        *ret = s;
        ফেরত 0;
}

ssize_t string_table_lookup_from_string_fallback(ধ্রুবক অক্ষর * ধ্রুবক *table, size_t len, ধ্রুবক অক্ষর *s, size_t max) {
        যদি (!s)
                ফেরত -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        যদি (i >= 0)
                ফেরত i;

        unsigned u;
        যদি (safe_atou(s, &u) < 0)
                ফেরত -EINVAL;
        যদি (u > max)
                ফেরত -EINVAL;

        ফেরত u;
}
