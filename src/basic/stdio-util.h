/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <printf.h>
#شامل <stdio.h>

#شامل "forward.h"

_printf_(3, 4)
ایستا درونخطی نویسه* snprintf_ok(نویسه *buf, size_t len, ثابت نویسه *format, ...) {
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        بازگشت r >= 0 && (size_t) r < len ? buf : پوچ;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

نویسه* asprintf_safe(ثابت نویسه *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
انجامبده {                                                                    \
        صحیح _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        اگر (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, اندازه(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        أكد(_k < ELEMENTSOF(_argtypes));                             \
        برای (_i = 0; _i < _k; _i++) {                                   \
                اگر (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (تهی) va_arg(ap, تهی*);                       \
                        ادامه;                                       \
                }                                                       \
                                                                        \
                انتخاب (_argtypes[_i]) {                                \
                حالت PA_INT:                                            \
                حالت PA_INT|PA_FLAG_SHORT:                              \
                حالت PA_CHAR:                                           \
                        (تهی) va_arg(ap, صحیح);                         \
                        بشکن;                                          \
                حالت PA_INT|PA_FLAG_LONG:                               \
                        (تهی) va_arg(ap, long صحیح);                    \
                        بشکن;                                          \
                حالت PA_INT|PA_FLAG_LONG_LONG:                          \
                        (تهی) va_arg(ap, long long صحیح);               \
                        بشکن;                                          \
                حالت PA_WCHAR:                                          \
                        (تهی) va_arg(ap, wchar_t);                     \
                        بشکن;                                          \
                حالت PA_WSTRING:                                        \
                حالت PA_STRING:                                         \
                حالت PA_POINTER:                                        \
                        (تهی) va_arg(ap, تهی*);                       \
                        بشکن;                                          \
                حالت PA_FLOAT:                                          \
                حالت PA_DOUBLE:                                         \
                        (تهی) va_arg(ap, دوگانه);                      \
                        بشکن;                                          \
                حالت PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (تهی) va_arg(ap, long دوگانه);                 \
                        بشکن;                                          \
                پیشفرض:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} وقتی (false)
