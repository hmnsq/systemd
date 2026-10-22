/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <printf.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

_printf_(3, 4)
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* snprintf_ok(ಅಕ್ಷರ *buf, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r >= 0 && (size_t) r < len ? buf : ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು xsprintf(buf, fmt, ...) \
        assert_ಸಂದೇಶ_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

ಅಕ್ಷರ* asprintf_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸೀಮಿತ fmt, ...) _printf_(1, 2);

#ವ್ಯಾಖ್ಯಾನಿಸು VA_FORMAT_ADVANCE(format, ap)                                   \
ಮಾಡು {                                                                    \
        ಪೂರ್ಣಾಂಕ _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        ಆದರೆ (HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER)                               \
                memset(_argtypes, 0, ಗಾತ್ರ(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        ಪ್ರತಿಜ್ಞಾ(_k < ELEMENTSOF(_argtypes));                             \
        ಗಾಗಿ (_i = 0; _i < _k; _i++) {                                   \
                ಆದರೆ (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (ಶೂನ್ಯ) va_arg(ap, ಶೂನ್ಯ*);                       \
                        ಮುಂದುವರಿ;                                       \
                }                                                       \
                                                                        \
                ಆಯ್ಕೆ (_argtypes[_i]) {                                \
                ಸ್ಥಿತಿ PA_INT:                                            \
                ಸ್ಥಿತಿ PA_INT|PA_FLAG_SHORT:                              \
                ಸ್ಥಿತಿ PA_CHAR:                                           \
                        (ಶೂನ್ಯ) va_arg(ap, ಪೂರ್ಣಾಂಕ);                         \
                        ನಿಲ್ಲಿಸು;                                          \
                ಸ್ಥಿತಿ PA_INT|PA_FLAG_LONG:                               \
                        (ಶೂನ್ಯ) va_arg(ap, ದೀರ್ಘ ಪೂರ್ಣಾಂಕ);                    \
                        ನಿಲ್ಲಿಸು;                                          \
                ಸ್ಥಿತಿ PA_INT|PA_FLAG_LONG_LONG:                          \
                        (ಶೂನ್ಯ) va_arg(ap, ದೀರ್ಘ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ);               \
                        ನಿಲ್ಲಿಸು;                                          \
                ಸ್ಥಿತಿ PA_WCHAR:                                          \
                        (ಶೂನ್ಯ) va_arg(ap, wchar_t);                     \
                        ನಿಲ್ಲಿಸು;                                          \
                ಸ್ಥಿತಿ PA_WSTRING:                                        \
                ಸ್ಥಿತಿ PA_STRING:                                         \
                ಸ್ಥಿತಿ PA_POINTER:                                        \
                        (ಶೂನ್ಯ) va_arg(ap, ಶೂನ್ಯ*);                       \
                        ನಿಲ್ಲಿಸು;                                          \
                ಸ್ಥಿತಿ PA_FLOAT:                                          \
                ಸ್ಥಿತಿ PA_DOUBLE:                                         \
                        (ಶೂನ್ಯ) va_arg(ap, ದ್ವಿಗುಣ);                      \
                        ನಿಲ್ಲಿಸು;                                          \
                ಸ್ಥಿತಿ PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (ಶೂನ್ಯ) va_arg(ap, ದೀರ್ಘ ದ್ವಿಗುಣ);                 \
                        ನಿಲ್ಲಿಸು;                                          \
                ಪೂರ್ವನಿಯೋಜಿತ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} ಆಗ (false)
