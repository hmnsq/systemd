/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <printf.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "forward.h"

_printf_(3, 4)
স্থির অন্তঃরেখ অক্ষর* snprintf_ok(অক্ষর *buf, size_t len, ধ্রুবক অক্ষর *format, ...) {
        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ফেরত r >= 0 && (size_t) r < len ? buf : ফাঁকা;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

অক্ষর* asprintf_safe(ধ্রুবক অক্ষর *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
করো {                                                                    \
        পূর্ণসংখ্যা _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        যদি (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, আকার(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        প্রতিজ্ঞা(_k < ELEMENTSOF(_argtypes));                             \
        জন্য (_i = 0; _i < _k; _i++) {                                   \
                যদি (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (শূন্য) va_arg(ap, শূন্য*);                       \
                        চালু;                                       \
                }                                                       \
                                                                        \
                নির্বাচন (_argtypes[_i]) {                                \
                ক্ষেত্র PA_INT:                                            \
                ক্ষেত্র PA_INT|PA_FLAG_SHORT:                              \
                ক্ষেত্র PA_CHAR:                                           \
                        (শূন্য) va_arg(ap, পূর্ণসংখ্যা);                         \
                        বিরতি;                                          \
                ক্ষেত্র PA_INT|PA_FLAG_LONG:                               \
                        (শূন্য) va_arg(ap, long পূর্ণসংখ্যা);                    \
                        বিরতি;                                          \
                ক্ষেত্র PA_INT|PA_FLAG_LONG_LONG:                          \
                        (শূন্য) va_arg(ap, long long পূর্ণসংখ্যা);               \
                        বিরতি;                                          \
                ক্ষেত্র PA_WCHAR:                                          \
                        (শূন্য) va_arg(ap, wchar_t);                     \
                        বিরতি;                                          \
                ক্ষেত্র PA_WSTRING:                                        \
                ক্ষেত্র PA_STRING:                                         \
                ক্ষেত্র PA_POINTER:                                        \
                        (শূন্য) va_arg(ap, শূন্য*);                       \
                        বিরতি;                                          \
                ক্ষেত্র PA_FLOAT:                                          \
                ক্ষেত্র PA_DOUBLE:                                         \
                        (শূন্য) va_arg(ap, দ্বিগুণ);                      \
                        বিরতি;                                          \
                ক্ষেত্র PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (শূন্য) va_arg(ap, long দ্বিগুণ);                 \
                        বিরতি;                                          \
                পূর্বনির্ধারিত:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} যখন (false)
