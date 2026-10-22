/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <printf.h>
#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "forward.h"

_printf_(3, 4)
স্থির অন্তঃস্থ ৱর্ণ* snprintf_ok(ৱর্ণ *buf, size_t len, ধ্রুৱ ৱর্ণ *format, ...) {
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ঘূৰাই r >= 0 && (size_t) r < len ? buf : রিক্ত;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

ৱর্ণ* asprintf_safe(ধ্রুৱ ৱর্ণ *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
কৰা {                                                                    \
        পূর্ণাঙ্ক _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        যদি (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, পরিমাণ(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        প্রতিজ্ঞা(_k < ELEMENTSOF(_argtypes));                             \
        বাবে (_i = 0; _i < _k; _i++) {                                   \
                যদি (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (শূন্য) va_arg(ap, শূন্য*);                       \
                        অনুৱর্ত;                                       \
                }                                                       \
                                                                        \
                ৱিকল্প (_argtypes[_i]) {                                \
                পক্ষ PA_INT:                                            \
                পক্ষ PA_INT|PA_FLAG_SHORT:                              \
                পক্ষ PA_CHAR:                                           \
                        (শূন্য) va_arg(ap, পূর্ণাঙ্ক);                         \
                        ভঙ্গ;                                          \
                পক্ষ PA_INT|PA_FLAG_LONG:                               \
                        (শূন্য) va_arg(ap, long পূর্ণাঙ্ক);                    \
                        ভঙ্গ;                                          \
                পক্ষ PA_INT|PA_FLAG_LONG_LONG:                          \
                        (শূন্য) va_arg(ap, long long পূর্ণাঙ্ক);               \
                        ভঙ্গ;                                          \
                পক্ষ PA_WCHAR:                                          \
                        (শূন্য) va_arg(ap, wchar_t);                     \
                        ভঙ্গ;                                          \
                পক্ষ PA_WSTRING:                                        \
                পক্ষ PA_STRING:                                         \
                পক্ষ PA_POINTER:                                        \
                        (শূন্য) va_arg(ap, শূন্য*);                       \
                        ভঙ্গ;                                          \
                পক্ষ PA_FLOAT:                                          \
                পক্ষ PA_DOUBLE:                                         \
                        (শূন্য) va_arg(ap, দ্ৱিগুণ);                      \
                        ভঙ্গ;                                          \
                পক্ষ PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (শূন্য) va_arg(ap, long দ্ৱিগুণ);                 \
                        ভঙ্গ;                                          \
                স্ৱভাৱ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} যেতিয়ালৈ (false)
