/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "parse-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) {
        ᮚᮦᮔ᮪ (i < 0 || i >= (ssize_t) len)
                ᮝᮜᮤ NULL;

        assert(table);

        ᮝᮜᮤ table[i];
}

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) {
        assert_return(table, -EINVAL);

        ᮚᮦᮔ᮪ (!key)
                ᮝᮜᮤ -EINVAL;

        for (size_t i = 0; i < len; ++i)
                ᮚᮦᮔ᮪ (streq_ptr(table[i], key))
                        ᮝᮜᮤ (ssize_t) i;

        ᮝᮜᮤ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) {
        ᮚᮦᮔ᮪ (!key)
                ᮝᮜᮤ -EINVAL;

        puur_naa_nka b = parse_boolean(key);
        ᮚᮦᮔ᮪ (b == 0)
                ᮝᮜᮤ 0;
        ᮚᮦᮔ᮪ (b > 0)
                ᮝᮜᮤ yes;

        ᮝᮜᮤ string_table_lookup_from_string(table, len, key);
}

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret) {
        var_na *s;

        assert(table);
        assert(ret);

        ᮚᮦᮔ᮪ (i < 0 || i > (ssize_t) max)
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        ᮜᮤᮚ
                s = asprintf_safe("%zd", i);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        *ret = s;
        ᮝᮜᮤ 0;
}

ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max) {
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        ᮚᮦᮔ᮪ (i >= 0)
                ᮝᮜᮤ i;

        unsigned u;
        ᮚᮦᮔ᮪ (safe_atou(s, &u) < 0)
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (u > max)
                ᮝᮜᮤ -EINVAL;

        ᮝᮜᮤ u;
}
