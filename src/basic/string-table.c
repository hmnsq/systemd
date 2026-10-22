/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"

अचर वर्ण* string_table_lookup_to_string(अचर वर्ण * अचर *table, माप_प्रकार len, ssize_t i) {
        यदि (i < 0 || i >= (ssize_t) len)
                फर्काऊ रिक्त;

        प्रतिज्ञा(table);

        फर्काऊ table[i];
}

ssize_t string_table_lookup_from_string(अचर वर्ण * अचर *table, माप_प्रकार len, अचर वर्ण *साँचो) {
        assert_return(table, -EINVAL);

        यदि (!साँचो)
                फर्काऊ -EINVAL;

        लागि (माप_प्रकार i = 0; i < len; ++i)
                यदि (streq_ptr(table[i], साँचो))
                        फर्काऊ (ssize_t) i;

        फर्काऊ -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(अचर वर्ण * अचर *table, माप_प्रकार len, अचर वर्ण *साँचो, ssize_t yes) {
        यदि (!साँचो)
                फर्काऊ -EINVAL;

        पूर्णाङ्क b = parse_boolean(साँचो);
        यदि (b == 0)
                फर्काऊ 0;
        यदि (b > 0)
                फर्काऊ yes;

        फर्काऊ string_table_lookup_from_string(table, len, साँचो);
}

पूर्णाङ्क string_table_lookup_to_string_fallback(अचर वर्ण * अचर *table, माप_प्रकार len, ssize_t i, माप_प्रकार max, वर्ण **ret) {
        वर्ण *s;

        प्रतिज्ञा(table);
        प्रतिज्ञा(ret);

        यदि (i < 0 || i > (ssize_t) max)
                फर्काऊ -दुस्फल;

        यदि (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        अन्यथा
                s = asprintf_safe("%zd", i);
        यदि (!s)
                फर्काऊ -ENOMEM;

        *ret = s;
        फर्काऊ 0;
}

ssize_t string_table_lookup_from_string_fallback(अचर वर्ण * अचर *table, माप_प्रकार len, अचर वर्ण *s, माप_प्रकार max) {
        यदि (!s)
                फर्काऊ -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        यदि (i >= 0)
                फर्काऊ i;

        अचिन्हित u;
        यदि (safe_atou(s, &u) < 0)
                फर्काऊ -EINVAL;
        यदि (u > max)
                फर्काऊ -EINVAL;

        फर्काऊ u;
}
