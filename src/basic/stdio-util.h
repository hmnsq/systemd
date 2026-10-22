/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <printf.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "forward.h"

_printf_(3, 4)
स्थिर ओळीत अक्षर* snprintf_ok(अक्षर *buf, माप_प्रकार len, स्थिरांक अक्षर *format, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vsnprintf(buf, len, format, ap);
        बहु_पूर्ण(ap);

        परत r >= 0 && (माप_प्रकार) r < len ? buf : रिक्त;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

अक्षर* asprintf_safe(स्थिरांक अक्षर *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
करा {                                                                    \
        पूर्णांक _argtypes[128];                                             \
        माप_प्रकार _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        जर (HAS_FEATURE_MEMORY_SANITIZER)                               \
                स_रखो(_argtypes, 0, आकार(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        प्रतिज्ञा(_k < ELEMENTSOF(_argtypes));                             \
        साठी (_i = 0; _i < _k; _i++) {                                   \
                जर (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (शून्य) बहु_तर्क(ap, शून्य*);                       \
                        सुरूठेवा;                                       \
                }                                                       \
                                                                        \
                निवड (_argtypes[_i]) {                                \
                स्थिती PA_INT:                                            \
                स्थिती PA_INT|PA_FLAG_SHORT:                              \
                स्थिती PA_CHAR:                                           \
                        (शून्य) बहु_तर्क(ap, पूर्णांक);                         \
                        थांबा;                                          \
                स्थिती PA_INT|PA_FLAG_LONG:                               \
                        (शून्य) बहु_तर्क(ap, दीर्घ पूर्णांक);                    \
                        थांबा;                                          \
                स्थिती PA_INT|PA_FLAG_LONG_LONG:                          \
                        (शून्य) बहु_तर्क(ap, दीर्घ दीर्घ पूर्णांक);               \
                        थांबा;                                          \
                स्थिती PA_WCHAR:                                          \
                        (शून्य) बहु_तर्क(ap, ब_अक्षर_प्रकार);                     \
                        थांबा;                                          \
                स्थिती PA_WSTRING:                                        \
                स्थिती PA_STRING:                                         \
                स्थिती PA_POINTER:                                        \
                        (शून्य) बहु_तर्क(ap, शून्य*);                       \
                        थांबा;                                          \
                स्थिती PA_FLOAT:                                          \
                स्थिती PA_DOUBLE:                                         \
                        (शून्य) बहु_तर्क(ap, द्विगुण);                      \
                        थांबा;                                          \
                स्थिती PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (शून्य) बहु_तर्क(ap, दीर्घ द्विगुण);                 \
                        थांबा;                                          \
                पूर्वनिर्धारित:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} पर्यंत (false)
