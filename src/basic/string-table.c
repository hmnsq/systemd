/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"

स्थिरांक अक्षर* string_table_lookup_to_string(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, ssize_t i) {
        जर (i < 0 || i >= (ssize_t) len)
                परत रिक्त;

        प्रतिज्ञा(table);

        परत table[i];
}

ssize_t string_table_lookup_from_string(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, स्थिरांक अक्षर *किल्ली) {
        assert_return(table, -EINVAL);

        जर (!किल्ली)
                परत -EINVAL;

        साठी (माप_प्रकार i = 0; i < len; ++i)
                जर (streq_ptr(table[i], किल्ली))
                        परत (ssize_t) i;

        परत -EINVAL;
}

ssize_t string_table_lookup_from_string_with_boolean(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, स्थिरांक अक्षर *किल्ली, ssize_t yes) {
        जर (!किल्ली)
                परत -EINVAL;

        पूर्णांक b = parse_boolean(किल्ली);
        जर (b == 0)
                परत 0;
        जर (b > 0)
                परत yes;

        परत string_table_lookup_from_string(table, len, किल्ली);
}

पूर्णांक string_table_lookup_to_string_fallback(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, ssize_t i, माप_प्रकार max, अक्षर **ret) {
        अक्षर *s;

        प्रतिज्ञा(table);
        प्रतिज्ञा(ret);

        जर (i < 0 || i > (ssize_t) max)
                परत -दुस्फल;

        जर (i < (ssize_t) len && table[i])
                s = strdup(table[i]);
        अन्यथा
                s = asprintf_safe("%zd", i);
        जर (!s)
                परत -ENOMEM;

        *ret = s;
        परत 0;
}

ssize_t string_table_lookup_from_string_fallback(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, स्थिरांक अक्षर *s, माप_प्रकार max) {
        जर (!s)
                परत -EINVAL;

        ssize_t i = string_table_lookup_from_string(table, len, s);
        जर (i >= 0)
                परत i;

        अचिन्हित u;
        जर (safe_atou(s, &u) < 0)
                परत -EINVAL;
        जर (u > max)
                परत -EINVAL;

        परत u;
}
