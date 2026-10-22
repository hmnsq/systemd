/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <printf.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "forward.h"

_printf_(3, 4)
સ્થિર અંતઃરેખ અક્ષર* snprintf_ok(અક્ષર *buf, size_t len, અચલ અક્ષર *format, ...) {
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        પાછા r >= 0 && (size_t) r < len ? buf : ખાલી;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

અક્ષર* asprintf_safe(અચલ અક્ષર *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
કરો {                                                                    \
        પૂર્ણાંક _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        જો (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, કદ(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        પ્રતિજ્ઞા(_k < ELEMENTSOF(_argtypes));                             \
        માટે (_i = 0; _i < _k; _i++) {                                   \
                જો (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (શૂન્ય) va_arg(ap, શૂન્ય*);                       \
                        ચાલુ;                                       \
                }                                                       \
                                                                        \
                પસંદગી (_argtypes[_i]) {                                \
                સ્થિતિ PA_INT:                                            \
                સ્થિતિ PA_INT|PA_FLAG_SHORT:                              \
                સ્થિતિ PA_CHAR:                                           \
                        (શૂન્ય) va_arg(ap, પૂર્ણાંક);                         \
                        તોડો;                                          \
                સ્થિતિ PA_INT|PA_FLAG_LONG:                               \
                        (શૂન્ય) va_arg(ap, long પૂર્ણાંક);                    \
                        તોડો;                                          \
                સ્થિતિ PA_INT|PA_FLAG_LONG_LONG:                          \
                        (શૂન્ય) va_arg(ap, long long પૂર્ણાંક);               \
                        તોડો;                                          \
                સ્થિતિ PA_WCHAR:                                          \
                        (શૂન્ય) va_arg(ap, wchar_t);                     \
                        તોડો;                                          \
                સ્થિતિ PA_WSTRING:                                        \
                સ્થિતિ PA_STRING:                                         \
                સ્થિતિ PA_POINTER:                                        \
                        (શૂન્ય) va_arg(ap, શૂન્ય*);                       \
                        તોડો;                                          \
                સ્થિતિ PA_FLOAT:                                          \
                સ્થિતિ PA_DOUBLE:                                         \
                        (શૂન્ય) va_arg(ap, દ્વિગુણ);                      \
                        તોડો;                                          \
                સ્થિતિ PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (શૂન્ય) va_arg(ap, long દ્વિગુણ);                 \
                        તોડો;                                          \
                મૂળભૂત:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} જ્યારે (false)
