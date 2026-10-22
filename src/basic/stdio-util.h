/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <printf.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

_printf_(3, 4)
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* snprintf_ok(ଅକ୍ଷର *buf, size_t len, ଅଚଳ ଅକ୍ଷର *format, ...) {
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r >= 0 && (size_t) r < len ? buf : ଖାଲି;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

ଅକ୍ଷର* asprintf_safe(ଅଚଳ ଅକ୍ଷର *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
କର {                                                                    \
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        ଯଦି (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, ଆକାର(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        ପ୍ରତିଜ୍ଞା(_k < ELEMENTSOF(_argtypes));                             \
        ପାଇଁ (_i = 0; _i < _k; _i++) {                                   \
                ଯଦି (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (ଶୂନ୍ୟ) va_arg(ap, ଶୂନ୍ୟ*);                       \
                        ଜାରି;                                       \
                }                                                       \
                                                                        \
                ବାଛ (_argtypes[_i]) {                                \
                ସ୍ଥିତି PA_INT:                                            \
                ସ୍ଥିତି PA_INT|PA_FLAG_SHORT:                              \
                ସ୍ଥିତି PA_CHAR:                                           \
                        (ଶୂନ୍ୟ) va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);                         \
                        ଭାଙ୍ଗ;                                          \
                ସ୍ଥିତି PA_INT|PA_FLAG_LONG:                               \
                        (ଶୂନ୍ୟ) va_arg(ap, long ପୂର୍ଣ୍ଣସଂଖ୍ୟା);                    \
                        ଭାଙ୍ଗ;                                          \
                ସ୍ଥିତି PA_INT|PA_FLAG_LONG_LONG:                          \
                        (ଶୂନ୍ୟ) va_arg(ap, long long ପୂର୍ଣ୍ଣସଂଖ୍ୟା);               \
                        ଭାଙ୍ଗ;                                          \
                ସ୍ଥିତି PA_WCHAR:                                          \
                        (ଶୂନ୍ୟ) va_arg(ap, wchar_t);                     \
                        ଭାଙ୍ଗ;                                          \
                ସ୍ଥିତି PA_WSTRING:                                        \
                ସ୍ଥିତି PA_STRING:                                         \
                ସ୍ଥିତି PA_POINTER:                                        \
                        (ଶୂନ୍ୟ) va_arg(ap, ଶୂନ୍ୟ*);                       \
                        ଭାଙ୍ଗ;                                          \
                ସ୍ଥିତି PA_FLOAT:                                          \
                ସ୍ଥିତି PA_DOUBLE:                                         \
                        (ଶୂନ୍ୟ) va_arg(ap, ଦ୍ୱିଗୁଣ);                      \
                        ଭାଙ୍ଗ;                                          \
                ସ୍ଥିତି PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (ଶୂନ୍ୟ) va_arg(ap, long ଦ୍ୱିଗୁଣ);                 \
                        ଭାଙ୍ଗ;                                          \
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} ଯେତେବେଳେ (false)
