/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <printf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

_printf_(3, 4)
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* snprintf_ok(ਅੱਖਰ *buf, size_t len, ਅਚਲ ਅੱਖਰ *format, ...) {
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ਵਾਪਸ r >= 0 && (size_t) r < len ? buf : ਨਿਰਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xsprintf(buf, fmt, ...) \
        assert_ਸੁਨੇਹਾ_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

ਅੱਖਰ* asprintf_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਸੀਮਿਤ fmt, ...) _printf_(1, 2);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VA_FORMAT_ADVANCE(format, ap)                                   \
ਕਰੋ {                                                                    \
        ਪੂਰਨ _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        ਜੇ (HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER)                               \
                memset(_argtypes, 0, ਆਕਾਰ(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        ਪ੍ਰਤਿਜ੍ਞਾ(_k < ELEMENTSOF(_argtypes));                             \
        ਲਈ (_i = 0; _i < _k; _i++) {                                   \
                ਜੇ (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (ਖਾਲੀ) va_arg(ap, ਖਾਲੀ*);                       \
                        ਜਾਰੀ;                                       \
                }                                                       \
                                                                        \
                ਚੋਣ (_argtypes[_i]) {                                \
                ਹਾਲਤ PA_INT:                                            \
                ਹਾਲਤ PA_INT|PA_FLAG_SHORT:                              \
                ਹਾਲਤ PA_CHAR:                                           \
                        (ਖਾਲੀ) va_arg(ap, ਪੂਰਨ);                         \
                        ਰੋਕੋ;                                          \
                ਹਾਲਤ PA_INT|PA_FLAG_LONG:                               \
                        (ਖਾਲੀ) va_arg(ap, ਲੰਮਾ ਪੂਰਨ);                    \
                        ਰੋਕੋ;                                          \
                ਹਾਲਤ PA_INT|PA_FLAG_LONG_LONG:                          \
                        (ਖਾਲੀ) va_arg(ap, ਲੰਮਾ ਲੰਮਾ ਪੂਰਨ);               \
                        ਰੋਕੋ;                                          \
                ਹਾਲਤ PA_WCHAR:                                          \
                        (ਖਾਲੀ) va_arg(ap, wchar_t);                     \
                        ਰੋਕੋ;                                          \
                ਹਾਲਤ PA_WSTRING:                                        \
                ਹਾਲਤ PA_STRING:                                         \
                ਹਾਲਤ PA_POINTER:                                        \
                        (ਖਾਲੀ) va_arg(ap, ਖਾਲੀ*);                       \
                        ਰੋਕੋ;                                          \
                ਹਾਲਤ PA_FLOAT:                                          \
                ਹਾਲਤ PA_DOUBLE:                                         \
                        (ਖਾਲੀ) va_arg(ap, ਦੁੱਗਣਾ);                      \
                        ਰੋਕੋ;                                          \
                ਹਾਲਤ PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (ਖਾਲੀ) va_arg(ap, ਲੰਮਾ ਦੁੱਗਣਾ);                 \
                        ਰੋਕੋ;                                          \
                ਮੂਲ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} ਜਦੋਂ (false)
