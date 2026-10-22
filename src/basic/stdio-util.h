/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <printf.h>
#include <stdio.h>

#include "forward.h"

_printf_(3, 4)
static inline char* snprintf_ok(char *buf, size_t len, const char *format, ...) {
        va_list ap;
        정수 r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        반환 r >= 0 && (size_t) r < len ? buf : NULL;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

char* asprintf_safe(const char *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
do {                                                                    \
        정수 _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        만약 (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, sizeof(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        assert(_k < ELEMENTSOF(_argtypes));                             \
        위해 (_i = 0; _i < _k; _i++) {                                   \
                만약 (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (void) va_arg(ap, void*);                       \
                        continue;                                       \
                }                                                       \
                                                                        \
                switch (_argtypes[_i]) {                                \
                case PA_INT:                                            \
                case PA_INT|PA_FLAG_SHORT:                              \
                case PA_CHAR:                                           \
                        (void) va_arg(ap, 정수);                         \
                        break;                                          \
                case PA_INT|PA_FLAG_LONG:                               \
                        (void) va_arg(ap, long 정수);                    \
                        break;                                          \
                case PA_INT|PA_FLAG_LONG_LONG:                          \
                        (void) va_arg(ap, long long 정수);               \
                        break;                                          \
                case PA_WCHAR:                                          \
                        (void) va_arg(ap, wchar_t);                     \
                        break;                                          \
                case PA_WSTRING:                                        \
                case PA_STRING:                                         \
                case PA_POINTER:                                        \
                        (void) va_arg(ap, void*);                       \
                        break;                                          \
                case PA_FLOAT:                                          \
                case PA_DOUBLE:                                         \
                        (void) va_arg(ap, double);                      \
                        break;                                          \
                case PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (void) va_arg(ap, long double);                 \
                        break;                                          \
                default:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} 동안 (false)
