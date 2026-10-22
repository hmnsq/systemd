/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <printf.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

_printf_(3, 4)
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* snprintf_ok(ವರ್ಣ *buf, size_t len, ಧ್ರುವ ವರ್ಣ *format, ...) {
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r >= 0 && (size_t) r < len ? buf : ರಿಕ್ತ;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

ವರ್ಣ* asprintf_safe(ಧ್ರುವ ವರ್ಣ *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
ಕುರು {                                                                    \
        ಪೂರ್ಣಾಙ್ಕ _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        ಯದಿ (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, ಪರಿಮಾಣ(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        ಪ್ರತಿಜ್ಞಾ(_k < ELEMENTSOF(_argtypes));                             \
        ಪ್ರತಿ (_i = 0; _i < _k; _i++) {                                   \
                ಯದಿ (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (ಶೂನ್ಯ) va_arg(ap, ಶೂನ್ಯ*);                       \
                        ಅನುವರ್ತ;                                       \
                }                                                       \
                                                                        \
                ವಿಕಲ್ಪ (_argtypes[_i]) {                                \
                ಪಕ್ಷ PA_INT:                                            \
                ಪಕ್ಷ PA_INT|PA_FLAG_SHORT:                              \
                ಪಕ್ಷ PA_CHAR:                                           \
                        (ಶೂನ್ಯ) va_arg(ap, ಪೂರ್ಣಾಙ್ಕ);                         \
                        ಭಙ್ಗ;                                          \
                ಪಕ್ಷ PA_INT|PA_FLAG_LONG:                               \
                        (ಶೂನ್ಯ) va_arg(ap, long ಪೂರ್ಣಾಙ್ಕ);                    \
                        ಭಙ್ಗ;                                          \
                ಪಕ್ಷ PA_INT|PA_FLAG_LONG_LONG:                          \
                        (ಶೂನ್ಯ) va_arg(ap, long long ಪೂರ್ಣಾಙ್ಕ);               \
                        ಭಙ್ಗ;                                          \
                ಪಕ್ಷ PA_WCHAR:                                          \
                        (ಶೂನ್ಯ) va_arg(ap, wchar_t);                     \
                        ಭಙ್ಗ;                                          \
                ಪಕ್ಷ PA_WSTRING:                                        \
                ಪಕ್ಷ PA_STRING:                                         \
                ಪಕ್ಷ PA_POINTER:                                        \
                        (ಶೂನ್ಯ) va_arg(ap, ಶೂನ್ಯ*);                       \
                        ಭಙ್ಗ;                                          \
                ಪಕ್ಷ PA_FLOAT:                                          \
                ಪಕ್ಷ PA_DOUBLE:                                         \
                        (ಶೂನ್ಯ) va_arg(ap, ದ್ವಿಗುಣ);                      \
                        ಭಙ್ಗ;                                          \
                ಪಕ್ಷ PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (ಶೂನ್ಯ) va_arg(ap, long ದ್ವಿಗುಣ);                 \
                        ಭಙ್ಗ;                                          \
                ಸ್ವಭಾವ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} ಯಾವತ್ (false)
