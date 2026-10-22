/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"

ধ্রুব বর্ণ* string_table_lookup_to_string(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ssize_t i) {
        যদি (i < 0 || i >= (ssize_t) len)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(table);

        প্রত্যাগম table[i];
}

ssize_t string_table_lookup_from_string(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ধ্রুব বর্ণ *চাবি) {
        assert_return(table, -EINVAL);

        যদি (!চাবি)
                প্রত্যাগম -EINVAL;

        প্রতি (size_t i = 0; i < len; ++i)
                যদি (streq_ptr(table[i], চাবি))
                        প্রত্যাগম (ssize_t) i;

        প্রত্যাগম -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ধ্রুব বর্ণ *চাবি, ssize_t yes) {
        যদি (!চাবি)
                প্রত্যাগম -EINVAL;

        পূর্ণাঙ্ক b = parse_boolean(চাবি);
        যদি (b == 0)
                প্রত্যাগম 0;
        যদি (b > 0)
                প্রত্যাগম yes;

        প্রত্যাগম string_table_lookup_from_string(table, len, চাবি);
}

পূর্ণাঙ্ক string_table_lookup_to_string_fallback(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ssize_t i, size_t max, বর্ণ **ret) {
        বর্ণ *s;

        প্রতিজ্ঞা(table);
        প্রতিজ্ঞা(ret);

        যদি (i < 0 || i > (ssize_t) max)
                প্রত্যাগম -ERANGE;

        যদি (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        অন্যথা
                s = asprintf_safe("%zd", i);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        *ret = s;
        প্রত্যাগম 0;
}

ssize_t string_table_lookup_from_string_fallback(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ধ্রুব বর্ণ *s, size_t max) {
        যদি (!s)
                প্রত্যাগম -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        যদি (i >= 0)
                প্রত্যাগম i;

        unsigned u;
        যদি (safe_atou(s, &u) < 0)
                প্রত্যাগম -EINVAL;
        যদি (u > max)
                প্রত্যাগম -EINVAL;

        প্রত্যাগম u;
}
