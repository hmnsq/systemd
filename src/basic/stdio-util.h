/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <printf.h>
#समावेश <stdio.h>

#समावेश "forward.h"

_printf_(3, 4)
अटल अंतरभूत अक्षर* snprintf_ok(अक्षर *buf, माप_प्रकार len, स्थिर अक्षर *format, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vsnprintf(buf, len, format, ap);
        बहु_पूर्ण(ap);

        वापस r >= 0 && (माप_प्रकार) r < len ? buf : शून्य;
}

#घोषणा xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

अक्षर* asprintf_safe(स्थिर अक्षर *restrict fmt, ...) _printf_(1, 2);

#घोषणा VA_FORMAT_ADVANCE(format, ap)                                   \
करो {                                                                    \
        पूर्णांक _argtypes[128];                                             \
        माप_प्रकार _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        अगर (HAS_FEATURE_MEMORY_SANITIZER)                               \
                स_रखो(_argtypes, 0, माप(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        निश्चित(_k < ELEMENTSOF(_argtypes));                             \
        क्रम (_i = 0; _i < _k; _i++) {                                   \
                अगर (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (व्योम) बहु_तर्क(ap, व्योम*);                       \
                        जारी;                                       \
                }                                                       \
                                                                        \
                चयन (_argtypes[_i]) {                                \
                हाल PA_INT:                                            \
                हाल PA_INT|PA_FLAG_SHORT:                              \
                हाल PA_CHAR:                                           \
                        (व्योम) बहु_तर्क(ap, पूर्णांक);                         \
                        अवरोध;                                          \
                हाल PA_INT|PA_FLAG_LONG:                               \
                        (व्योम) बहु_तर्क(ap, दीर्घ पूर्णांक);                    \
                        अवरोध;                                          \
                हाल PA_INT|PA_FLAG_LONG_LONG:                          \
                        (व्योम) बहु_तर्क(ap, दीर्घ दीर्घ पूर्णांक);               \
                        अवरोध;                                          \
                हाल PA_WCHAR:                                          \
                        (व्योम) बहु_तर्क(ap, ब_अक्षर_प्रकार);                     \
                        अवरोध;                                          \
                हाल PA_WSTRING:                                        \
                हाल PA_STRING:                                         \
                हाल PA_POINTER:                                        \
                        (व्योम) बहु_तर्क(ap, व्योम*);                       \
                        अवरोध;                                          \
                हाल PA_FLOAT:                                          \
                हाल PA_DOUBLE:                                         \
                        (व्योम) बहु_तर्क(ap, द्विगुन);                      \
                        अवरोध;                                          \
                हाल PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (व्योम) बहु_तर्क(ap, दीर्घ द्विगुन);                 \
                        अवरोध;                                          \
                शेष:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} जबतक (false)
