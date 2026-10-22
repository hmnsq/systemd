/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <printf.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "forward.h"

_printf_(3, 4)
நிலையான உள்வரி எழுத்து* snprintf_ok(எழுத்து *buf, size_t len, மாறிலி எழுத்து *format, ...) {
        va_list ap;
        முழு r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        திரும்பு r >= 0 && (size_t) r < len ? buf : வெற்று;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

எழுத்து* asprintf_safe(மாறிலி எழுத்து *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
செய் {                                                                    \
        முழு _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        எனில் (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, அளவு(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        ப்ரதிஜ்ஞா(_k < ELEMENTSOF(_argtypes));                             \
        க்கு (_i = 0; _i < _k; _i++) {                                   \
                எனில் (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (வெறுமை) va_arg(ap, வெறுமை*);                       \
                        தொடர்;                                       \
                }                                                       \
                                                                        \
                தேர்வு (_argtypes[_i]) {                                \
                நிலை PA_INT:                                            \
                நிலை PA_INT|PA_FLAG_SHORT:                              \
                நிலை PA_CHAR:                                           \
                        (வெறுமை) va_arg(ap, முழு);                         \
                        நிறுத்து;                                          \
                நிலை PA_INT|PA_FLAG_LONG:                               \
                        (வெறுமை) va_arg(ap, long முழு);                    \
                        நிறுத்து;                                          \
                நிலை PA_INT|PA_FLAG_LONG_LONG:                          \
                        (வெறுமை) va_arg(ap, long long முழு);               \
                        நிறுத்து;                                          \
                நிலை PA_WCHAR:                                          \
                        (வெறுமை) va_arg(ap, wchar_t);                     \
                        நிறுத்து;                                          \
                நிலை PA_WSTRING:                                        \
                நிலை PA_STRING:                                         \
                நிலை PA_POINTER:                                        \
                        (வெறுமை) va_arg(ap, வெறுமை*);                       \
                        நிறுத்து;                                          \
                நிலை PA_FLOAT:                                          \
                நிலை PA_DOUBLE:                                         \
                        (வெறுமை) va_arg(ap, இரட்டை);                      \
                        நிறுத்து;                                          \
                நிலை PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (வெறுமை) va_arg(ap, long இரட்டை);                 \
                        நிறுத்து;                                          \
                இயல்பு:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} போது (false)
