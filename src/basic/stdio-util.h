/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <printf.h>
#شامل <stdio.h>

#شامل "forward.h"

_printf_(3, 4)
ساکن اندرونی حرف* snprintf_ok(حرف *buf, size_t len, ثابت حرف *format, ...) {
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        واپس r >= 0 && (size_t) r < len ? buf : تہی;
}

#تعریف_کرو xsprintf(buf, fmt, ...) \
        assert_پیغام_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

حرف* asprintf_محفوظ(ثابت حرف *محدود fmt, ...) _printf_(1, 2);

#تعریف_کرو VA_FORMAT_ADVANCE(format, ap)                                   \
کرو {                                                                    \
        صحیح _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        اگر (HAS_FEATURE_حافظہ_SANITIZER)                               \
                memset(_argtypes, 0, حجم(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        أكد(_k < ELEMENTSOF(_argtypes));                             \
        برائے (_i = 0; _i < _k; _i++) {                                   \
                اگر (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (خالی) va_arg(ap, خالی*);                       \
                        جاری;                                       \
                }                                                       \
                                                                        \
                انتخاب (_argtypes[_i]) {                                \
                حالت PA_INT:                                            \
                حالت PA_INT|PA_FLAG_SHORT:                              \
                حالت PA_CHAR:                                           \
                        (خالی) va_arg(ap, صحیح);                         \
                        توڑو;                                          \
                حالت PA_INT|PA_FLAG_LONG:                               \
                        (خالی) va_arg(ap, طویل صحیح);                    \
                        توڑو;                                          \
                حالت PA_INT|PA_FLAG_LONG_LONG:                          \
                        (خالی) va_arg(ap, طویل طویل صحیح);               \
                        توڑو;                                          \
                حالت PA_WCHAR:                                          \
                        (خالی) va_arg(ap, wchar_t);                     \
                        توڑو;                                          \
                حالت PA_WSTRING:                                        \
                حالت PA_STRING:                                         \
                حالت PA_POINTER:                                        \
                        (خالی) va_arg(ap, خالی*);                       \
                        توڑو;                                          \
                حالت PA_FLOAT:                                          \
                حالت PA_DOUBLE:                                         \
                        (خالی) va_arg(ap, دوہرا);                      \
                        توڑو;                                          \
                حالت PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (خالی) va_arg(ap, طویل دوہرا);                 \
                        توڑو;                                          \
                طےشدہ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} جب (false)
