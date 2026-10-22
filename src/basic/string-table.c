/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ꦪꦼꦤ꧀ (i < 0 || i >= (ssize_t) len)
                ꦮꦭꦶ NULL;

        assert(table);

        ꦮꦭꦶ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ꦪꦼꦤ꧀ (!key)
                ꦮꦭꦶ -EINVAL;

        ꦏꦁꦒꦺꦴ (size_t i = 0; i < len; ++i)
                ꦪꦼꦤ꧀ (streq_ptr(table[i], key))
                        ꦮꦭꦶ (ssize_t) i;

        ꦮꦭꦶ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ꦪꦼꦤ꧀ (!key)
                ꦮꦭꦶ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ꦪꦼꦤ꧀ (b == 0)
                ꦮꦭꦶ 0;
        ꦪꦼꦤ꧀ (b > 0)
                ꦮꦭꦶ yes;

        ꦮꦭꦶ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ꦪꦼꦤ꧀ (i < 0 || i > (ssize_t) max)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ꦭꦶꦪ
                s = asprintf_safe("%zd", i);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        *ret = s;
        ꦮꦭꦶ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ꦪꦼꦤ꧀ (i >= 0)
                ꦮꦭꦶ i;

        unsigned u;
        ꦪꦼꦤ꧀ (safe_atou(s, &u) < 0)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (u > max)
                ꦮꦭꦶ -EINVAL;

        ꦮꦭꦶ u;
}
