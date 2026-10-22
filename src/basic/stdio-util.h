/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <printf.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "forward.h"

_printf_(3, 4)
स्थिर अन्तःस्थ वर्ण* snprintf_ok(वर्ण *buf, माप_प्रकार len, ध्रुव वर्ण *format, ...) {
        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, format);
        r = vsnprintf(buf, len, format, ap);
        बहु_पूर्ण(ap);

        प्रत्यागम r >= 0 && (माप_प्रकार) r < len ? buf : रिक्त;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

वर्ण* asprintf_safe(ध्रुव वर्ण *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
कुरु {                                                                    \
        पूर्णाङ्क _argtypes[128];                                             \
        माप_प्रकार _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        यदि (HAS_FEATURE_MEMORY_SANITIZER)                               \
                स_रखो(_argtypes, 0, परिमाण(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        प्रतिज्ञा(_k < ELEMENTSOF(_argtypes));                             \
        प्रति (_i = 0; _i < _k; _i++) {                                   \
                यदि (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (शून्य) बहु_तर्क(ap, शून्य*);                       \
                        अनुवर्त;                                       \
                }                                                       \
                                                                        \
                विकल्प (_argtypes[_i]) {                                \
                पक्ष PA_INT:                                            \
                पक्ष PA_INT|PA_FLAG_SHORT:                              \
                पक्ष PA_CHAR:                                           \
                        (शून्य) बहु_तर्क(ap, पूर्णाङ्क);                         \
                        भङ्ग;                                          \
                पक्ष PA_INT|PA_FLAG_LONG:                               \
                        (शून्य) बहु_तर्क(ap, दीर्घ पूर्णाङ्क);                    \
                        भङ्ग;                                          \
                पक्ष PA_INT|PA_FLAG_LONG_LONG:                          \
                        (शून्य) बहु_तर्क(ap, दीर्घ दीर्घ पूर्णाङ्क);               \
                        भङ्ग;                                          \
                पक्ष PA_WCHAR:                                          \
                        (शून्य) बहु_तर्क(ap, ब_अक्षर_प्रकार);                     \
                        भङ्ग;                                          \
                पक्ष PA_WSTRING:                                        \
                पक्ष PA_STRING:                                         \
                पक्ष PA_POINTER:                                        \
                        (शून्य) बहु_तर्क(ap, शून्य*);                       \
                        भङ्ग;                                          \
                पक्ष PA_FLOAT:                                          \
                पक्ष PA_DOUBLE:                                         \
                        (शून्य) बहु_तर्क(ap, द्विगुण);                      \
                        भङ्ग;                                          \
                पक्ष PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (शून्य) बहु_तर्क(ap, दीर्घ द्विगुण);                 \
                        भङ्ग;                                          \
                स्वभाव:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} यावत् (false)
