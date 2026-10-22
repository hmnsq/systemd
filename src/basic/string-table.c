/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"

ध्रुव वर्ण* string_table_lookup_to_string(ध्रुव वर्ण * ध्रुव *table, size_t len, ssize_t i) {
        सचे (i < 0 || i >= (ssize_t) len)
                पटिनिवत्त रिक्त;

        प्रतिज्ञा(table);

        पटिनिवत्त table[i];
}

ssize_t string_table_lookup_from_string(ध्रुव वर्ण * ध्रुव *table, size_t len, ध्रुव वर्ण *कुञ्चिका) {
        assert_return(table, -EINVAL);

        सचे (!कुञ्चिका)
                पटिनिवत्त -EINVAL;

        अत्थाय (size_t i = 0; i < len; ++i)
                सचे (streq_ptr(table[i], कुञ्चिका))
                        पटिनिवत्त (ssize_t) i;

        पटिनिवत्त -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(ध्रुव वर्ण * ध्रुव *table, size_t len, ध्रुव वर्ण *कुञ्चिका, ssize_t yes) {
        सचे (!कुञ्चिका)
                पटिनिवत्त -EINVAL;

        पूर्णाङ्क b = parse_boolean(कुञ्चिका);
        सचे (b == 0)
                पटिनिवत्त 0;
        सचे (b > 0)
                पटिनिवत्त yes;

        पटिनिवत्त string_table_lookup_from_string(table, len, कुञ्चिका);
}

पूर्णाङ्क string_table_lookup_to_string_fallback(ध्रुव वर्ण * ध्रुव *table, size_t len, ssize_t i, size_t max, वर्ण **ret) {
        वर्ण *s;

        प्रतिज्ञा(table);
        प्रतिज्ञा(ret);

        सचे (i < 0 || i > (ssize_t) max)
                पटिनिवत्त -ERANGE;

        सचे (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        अञ्ञथा
                s = asprintf_safe("%zd", i);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        *ret = s;
        पटिनिवत्त 0;
}

ssize_t string_table_lookup_from_string_fallback(ध्रुव वर्ण * ध्रुव *table, size_t len, ध्रुव वर्ण *s, size_t max) {
        सचे (!s)
                पटिनिवत्त -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        सचे (i >= 0)
                पटिनिवत्त i;

        unsigned u;
        सचे (safe_atou(s, &u) < 0)
                पटिनिवत्त -EINVAL;
        सचे (u > max)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त u;
}
