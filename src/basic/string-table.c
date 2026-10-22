/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>

#समावेश "parse-util.h"
#समावेश "stdio-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"

स्थिर अक्षर* string_table_lookup_to_string(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, ssize_t i) {
        अगर (i < 0 || i >= (ssize_t) len)
                वापस शून्य;

        निश्चित(table);

        वापस table[i];
}

ssize_t string_table_lookup_from_string(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, स्थिर अक्षर *कुंजी) {
        assert_return(table, -EINVAL);

        अगर (!कुंजी)
                वापस -EINVAL;

        क्रम (माप_प्रकार i = 0; i < len; ++i)
                अगर (streq_ptr(table[i], कुंजी))
                        वापस (ssize_t) i;

        वापस -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, स्थिर अक्षर *कुंजी, ssize_t yes) {
        अगर (!कुंजी)
                वापस -EINVAL;

        पूर्णांक b = parse_boolean(कुंजी);
        अगर (b == 0)
                वापस 0;
        अगर (b > 0)
                वापस yes;

        वापस string_table_lookup_from_string(table, len, कुंजी);
}

पूर्णांक string_table_lookup_to_string_fallback(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, ssize_t i, माप_प्रकार max, अक्षर **ret) {
        अक्षर *s;

        निश्चित(table);
        निश्चित(ret);

        अगर (i < 0 || i > (ssize_t) max)
                वापस -दुस्फल;

        अगर (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        अन्यथा
                s = asprintf_safe("%zd", i);
        अगर (!s)
                वापस -ENOMEM;

        *ret = s;
        वापस 0;
}

ssize_t string_table_lookup_from_string_fallback(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, स्थिर अक्षर *s, माप_प्रकार max) {
        अगर (!s)
                वापस -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        अगर (i >= 0)
                वापस i;

        अचिन्हित u;
        अगर (safe_atou(s, &u) < 0)
                वापस -EINVAL;
        अगर (u > max)
                वापस -EINVAL;

        वापस u;
}
