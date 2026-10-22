/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"

மாறிலி எழுத்து* string_table_lookup_to_string(மாறிலி எழுத்து * மாறிலி *table, size_t len, ssize_t i) {
        எனில் (i < 0 || i >= (ssize_t) len)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(table);

        திரும்பு table[i];
}

ssize_t string_table_lookup_from_string(மாறிலி எழுத்து * மாறிலி *table, size_t len, மாறிலி எழுத்து *சாவி) {
        assert_return(table, -EINVAL);

        எனில் (!சாவி)
                திரும்பு -EINVAL;

        க்கு (size_t i = 0; i < len; ++i)
                எனில் (streq_ptr(table[i], சாவி))
                        திரும்பு (ssize_t) i;

        திரும்பு -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(மாறிலி எழுத்து * மாறிலி *table, size_t len, மாறிலி எழுத்து *சாவி, ssize_t yes) {
        எனில் (!சாவி)
                திரும்பு -EINVAL;

        முழு b = parse_boolean(சாவி);
        எனில் (b == 0)
                திரும்பு 0;
        எனில் (b > 0)
                திரும்பு yes;

        திரும்பு string_table_lookup_from_string(table, len, சாவி);
}

முழு string_table_lookup_to_string_fallback(மாறிலி எழுத்து * மாறிலி *table, size_t len, ssize_t i, size_t max, எழுத்து **ret) {
        எழுத்து *s;

        ப்ரதிஜ்ஞா(table);
        ப்ரதிஜ்ஞா(ret);

        எனில் (i < 0 || i > (ssize_t) max)
                திரும்பு -ERANGE;

        எனில் (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        இல்லையெனில்
                s = asprintf_safe("%zd", i);
        எனில் (!s)
                திரும்பு -ENOMEM;

        *ret = s;
        திரும்பு 0;
}

ssize_t string_table_lookup_from_string_fallback(மாறிலி எழுத்து * மாறிலி *table, size_t len, மாறிலி எழுத்து *s, size_t max) {
        எனில் (!s)
                திரும்பு -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        எனில் (i >= 0)
                திரும்பு i;

        unsigned u;
        எனில் (safe_atou(s, &u) < 0)
                திரும்பு -EINVAL;
        எனில் (u > max)
                திரும்பு -EINVAL;

        திரும்பு u;
}
