/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <printf.h>
#ضمن <stdio.h>

#ضمن "forward.h"

_printf_(3, 4)
ساكن مضمن حرف* snprintf_ok(حرف *buf, size_t len, ثابت حرف *format, ...) {
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        إرجاع r >= 0 && (size_t) r < len ? buf : عدم;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

حرف* asprintf_safe(ثابت حرف *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
افعل {                                                                    \
        صحيح _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        إذا (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, حجم(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        أكد(_k < ELEMENTSOF(_argtypes));                             \
        لكل (_i = 0; _i < _k; _i++) {                                   \
                إذا (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (فراغ) va_arg(ap, فراغ*);                       \
                        استمر;                                       \
                }                                                       \
                                                                        \
                بدل (_argtypes[_i]) {                                \
                حالة PA_INT:                                            \
                حالة PA_INT|PA_FLAG_SHORT:                              \
                حالة PA_CHAR:                                           \
                        (فراغ) va_arg(ap, صحيح);                         \
                        اكسر;                                          \
                حالة PA_INT|PA_FLAG_LONG:                               \
                        (فراغ) va_arg(ap, long صحيح);                    \
                        اكسر;                                          \
                حالة PA_INT|PA_FLAG_LONG_LONG:                          \
                        (فراغ) va_arg(ap, long long صحيح);               \
                        اكسر;                                          \
                حالة PA_WCHAR:                                          \
                        (فراغ) va_arg(ap, wchar_t);                     \
                        اكسر;                                          \
                حالة PA_WSTRING:                                        \
                حالة PA_STRING:                                         \
                حالة PA_POINTER:                                        \
                        (فراغ) va_arg(ap, فراغ*);                       \
                        اكسر;                                          \
                حالة PA_FLOAT:                                          \
                حالة PA_DOUBLE:                                         \
                        (فراغ) va_arg(ap, مزدوج);                      \
                        اكسر;                                          \
                حالة PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (فراغ) va_arg(ap, long مزدوج);                 \
                        اكسر;                                          \
                افتراضي:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} بينما (false)
