/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ଅଚଳ ଅକ୍ଷର* string_table_lookup_to_string(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ssize_t i) {
        ଯଦି (i < 0 || i >= (ssize_t) len)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(table);

        ଫେରନ୍ତୁ table[i];
}

ssize_t string_table_lookup_from_string(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ଅଚଳ ଅକ୍ଷର *ଚାବି) {
        assert_return(table, -EINVAL);

        ଯଦି (!ଚାବି)
                ଫେରନ୍ତୁ -EINVAL;

        ପାଇଁ (size_t i = 0; i < len; ++i)
                ଯଦି (streq_ptr(table[i], ଚାବି))
                        ଫେରନ୍ତୁ (ssize_t) i;

        ଫେରନ୍ତୁ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ଅଚଳ ଅକ୍ଷର *ଚାବି, ssize_t yes) {
        ଯଦି (!ଚାବି)
                ଫେରନ୍ତୁ -EINVAL;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = parse_boolean(ଚାବି);
        ଯଦି (b == 0)
                ଫେରନ୍ତୁ 0;
        ଯଦି (b > 0)
                ଫେରନ୍ତୁ yes;

        ଫେରନ୍ତୁ string_table_lookup_from_string(table, len, ଚାବି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_table_lookup_to_string_fallback(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ssize_t i, size_t max, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(table);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (i < 0 || i > (ssize_t) max)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ଅନ୍ୟଥା
                s = asprintf_safe("%zd", i);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = s;
        ଫେରନ୍ତୁ 0;
}

ssize_t string_table_lookup_from_string_fallback(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ଅଚଳ ଅକ୍ଷର *s, size_t max) {
        ଯଦି (!s)
                ଫେରନ୍ତୁ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ଯଦି (i >= 0)
                ଫେରନ୍ତୁ i;

        unsigned u;
        ଯଦି (safe_atou(s, &u) < 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (u > max)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ u;
}
