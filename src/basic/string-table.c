/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"

ध्रुव वर्ण* string_सारणी_lookup_गन्तव्यम्_string(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ssize_t i) {
        यदि (i < 0 || i >= (ssize_t) len)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(सारणी);

        प्रत्यागम सारणी[i];
}

ssize_t string_सारणी_lookup_from_string(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ध्रुव वर्ण *कुञ्चिका) {
        assert_return(सारणी, -EINVAL);

        यदि (!कुञ्चिका)
                प्रत्यागम -EINVAL;

        प्रति (size_t i = 0; i < len; ++i)
                यदि (streq_ptr(सारणी[i], कुञ्चिका))
                        प्रत्यागम (ssize_t) i;

        प्रत्यागम -EINVAL;
}

ssize_t string_सारणी_lookup_from_string_with_boolean(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ध्रुव वर्ण *कुञ्चिका, ssize_t yes) {
        यदि (!कुञ्चिका)
                प्रत्यागम -EINVAL;

        पूर्णाङ्क b = parse_boolean(कुञ्चिका);
        यदि (b == 0)
                प्रत्यागम 0;
        यदि (b > 0)
                प्रत्यागम yes;

        प्रत्यागम string_सारणी_lookup_from_string(सारणी, len, कुञ्चिका);
}

पूर्णाङ्क string_सारणी_lookup_गन्तव्यम्_string_fallback(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ssize_t i, size_t max, वर्ण **ret) {
        वर्ण *s;

        प्रतिज्ञा(सारणी);
        प्रतिज्ञा(ret);

        यदि (i < 0 || i > (ssize_t) max)
                प्रत्यागम -ERANGE;

        यदि (i < (ssize_t) len && सारणी[i])
                s = strdup(सारणी[i]);
        अन्यथा
                s = asprintf_safe("%zd", i);
        यदि (!s)
                प्रत्यागम -ENOMEM;

        *ret = s;
        प्रत्यागम 0;
}

ssize_t string_सारणी_lookup_from_string_fallback(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ध्रुव वर्ण *s, size_t max) {
        यदि (!s)
                प्रत्यागम -EINVAL;

        ssize_t i = string_सारणी_lookup_from_string(सारणी, len, s);
        यदि (i >= 0)
                प्रत्यागम i;

        अचिह्नित u;
        यदि (safe_atou(s, &u) < 0)
                प्रत्यागम -EINVAL;
        यदि (u > max)
                प्रत्यागम -EINVAL;

        प्रत्यागम u;
}
