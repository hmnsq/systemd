/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <printf.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "forward.h"

_printf_(3, 4)
స్థిర అంతఃవరుస అక్షరం* snprintf_ok(అక్షరం *buf, size_t len, స్థిరరాశి అక్షరం *format, ...) {
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        తిరిగి r >= 0 && (size_t) r < len ? buf : ఖాళీ;
}

#నిర్వచించు xsprintf(buf, fmt, ...) \
        assert_సందేశం_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

అక్షరం* asprintf_సురక్షిత(స్థిరరాశి అక్షరం *పరిమిత fmt, ...) _printf_(1, 2);

#నిర్వచించు VA_FORMAT_ADVANCE(format, ap)                                   \
చేయి {                                                                    \
        పూర్ణాంకం _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        ఒకవేళ (HAS_FEATURE_జ్ఞాపకం_SANITIZER)                               \
                memset(_argtypes, 0, పరిమాణం(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        ప్రతిజ్ఞా(_k < ELEMENTSOF(_argtypes));                             \
        కొరకు (_i = 0; _i < _k; _i++) {                                   \
                ఒకవేళ (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (శూన్య) va_arg(ap, శూన్య*);                       \
                        కొనసాగు;                                       \
                }                                                       \
                                                                        \
                ఎంపిక (_argtypes[_i]) {                                \
                స్థితి PA_INT:                                            \
                స్థితి PA_INT|PA_FLAG_SHORT:                              \
                స్థితి PA_CHAR:                                           \
                        (శూన్య) va_arg(ap, పూర్ణాంకం);                         \
                        ఆపు;                                          \
                స్థితి PA_INT|PA_FLAG_LONG:                               \
                        (శూన్య) va_arg(ap, దీర్ఘ పూర్ణాంకం);                    \
                        ఆపు;                                          \
                స్థితి PA_INT|PA_FLAG_LONG_LONG:                          \
                        (శూన్య) va_arg(ap, దీర్ఘ దీర్ఘ పూర్ణాంకం);               \
                        ఆపు;                                          \
                స్థితి PA_WCHAR:                                          \
                        (శూన్య) va_arg(ap, wchar_t);                     \
                        ఆపు;                                          \
                స్థితి PA_WSTRING:                                        \
                స్థితి PA_STRING:                                         \
                స్థితి PA_POINTER:                                        \
                        (శూన్య) va_arg(ap, శూన్య*);                       \
                        ఆపు;                                          \
                స్థితి PA_FLOAT:                                          \
                స్థితి PA_DOUBLE:                                         \
                        (శూన్య) va_arg(ap, ద్విగుణ);                      \
                        ఆపు;                                          \
                స్థితి PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (శూన్య) va_arg(ap, దీర్ఘ ద్విగుణ);                 \
                        ఆపు;                                          \
                అప్రమేయ:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} అయితే (false)
