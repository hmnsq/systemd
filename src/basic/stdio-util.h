/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <printf.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "forward.h"

_printf_(3, 4)
സ്ഥിതം വരിയിൽ അക്ഷരം* snprintf_ok(അക്ഷരം *buf, size_t len, സ്ഥിരം അക്ഷരം *format, ...) {
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        തിരികെ r >= 0 && (size_t) r < len ? buf : ശൂന്യ;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

അക്ഷരം* asprintf_safe(സ്ഥിരം അക്ഷരം *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
ചെയ്യുക {                                                                    \
        പൂർണ്ണസംഖ്യ _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        എങ്കിൽ (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, വലുപ്പം(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        പ്രതിജ്ഞാ(_k < ELEMENTSOF(_argtypes));                             \
        വേണ്ടി (_i = 0; _i < _k; _i++) {                                   \
                എങ്കിൽ (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (ശൂന്യം) va_arg(ap, ശൂന്യം*);                       \
                        തുടരുക;                                       \
                }                                                       \
                                                                        \
                തിരഞ്ഞെടുപ്പ് (_argtypes[_i]) {                                \
                അവസ്ഥ PA_INT:                                            \
                അവസ്ഥ PA_INT|PA_FLAG_SHORT:                              \
                അവസ്ഥ PA_CHAR:                                           \
                        (ശൂന്യം) va_arg(ap, പൂർണ്ണസംഖ്യ);                         \
                        നിർത്തുക;                                          \
                അവസ്ഥ PA_INT|PA_FLAG_LONG:                               \
                        (ശൂന്യം) va_arg(ap, long പൂർണ്ണസംഖ്യ);                    \
                        നിർത്തുക;                                          \
                അവസ്ഥ PA_INT|PA_FLAG_LONG_LONG:                          \
                        (ശൂന്യം) va_arg(ap, long long പൂർണ്ണസംഖ്യ);               \
                        നിർത്തുക;                                          \
                അവസ്ഥ PA_WCHAR:                                          \
                        (ശൂന്യം) va_arg(ap, wchar_t);                     \
                        നിർത്തുക;                                          \
                അവസ്ഥ PA_WSTRING:                                        \
                അവസ്ഥ PA_STRING:                                         \
                അവസ്ഥ PA_POINTER:                                        \
                        (ശൂന്യം) va_arg(ap, ശൂന്യം*);                       \
                        നിർത്തുക;                                          \
                അവസ്ഥ PA_FLOAT:                                          \
                അവസ്ഥ PA_DOUBLE:                                         \
                        (ശൂന്യം) va_arg(ap, ഇരട്ട);                      \
                        നിർത്തുക;                                          \
                അവസ്ഥ PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (ശൂന്യം) va_arg(ap, long ഇരട്ട);                 \
                        നിർത്തുക;                                          \
                സ്ഥിരസ്ഥിതി:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} ആകുമ്പോൾ (false)
