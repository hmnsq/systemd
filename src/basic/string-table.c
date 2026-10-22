/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"

અચલ અક્ષર* string_table_lookup_to_string(અચલ અક્ષર * અચલ *table, size_t len, ssize_t i) {
        જો (i < 0 || i >= (ssize_t) len)
                પાછા ખાલી;

        પ્રતિજ્ઞા(table);

        પાછા table[i];
}

ssize_t string_table_lookup_from_string(અચલ અક્ષર * અચલ *table, size_t len, અચલ અક્ષર *ચાવી) {
        assert_return(table, -EINVAL);

        જો (!ચાવી)
                પાછા -EINVAL;

        માટે (size_t i = 0; i < len; ++i)
                જો (streq_ptr(table[i], ચાવી))
                        પાછા (ssize_t) i;

        પાછા -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(અચલ અક્ષર * અચલ *table, size_t len, અચલ અક્ષર *ચાવી, ssize_t yes) {
        જો (!ચાવી)
                પાછા -EINVAL;

        પૂર્ણાંક b = parse_boolean(ચાવી);
        જો (b == 0)
                પાછા 0;
        જો (b > 0)
                પાછા yes;

        પાછા string_table_lookup_from_string(table, len, ચાવી);
}

પૂર્ણાંક string_table_lookup_to_string_fallback(અચલ અક્ષર * અચલ *table, size_t len, ssize_t i, size_t max, અક્ષર **ret) {
        અક્ષર *s;

        પ્રતિજ્ઞા(table);
        પ્રતિજ્ઞા(ret);

        જો (i < 0 || i > (ssize_t) max)
                પાછા -ERANGE;

        જો (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        નહીંતર
                s = asprintf_safe("%zd", i);
        જો (!s)
                પાછા -ENOMEM;

        *ret = s;
        પાછા 0;
}

ssize_t string_table_lookup_from_string_fallback(અચલ અક્ષર * અચલ *table, size_t len, અચલ અક્ષર *s, size_t max) {
        જો (!s)
                પાછા -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        જો (i >= 0)
                પાછા i;

        unsigned u;
        જો (safe_atou(s, &u) < 0)
                પાછા -EINVAL;
        જો (u > max)
                પાછા -EINVAL;

        પાછા u;
}
