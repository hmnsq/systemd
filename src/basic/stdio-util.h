/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <printf.h>
#include <stdio.h>

#include "forward.h"

_printf_(3, 4)
static anta_hstha var_na* snprintf_ok(var_na *buf, size_t len, const var_na *format, ...) {
        va_list ap;
        puur_naa_nka r;

        va_start(ap, format);
        r = vsnprintf(buf, len, format, ap);
        va_end(ap);

        ᨀᨘᨒ r >= 0 && (size_t) r < len ? buf : NULL;
}

#define xsprintf(buf, fmt, ...) \
        assert_message_se(snprintf_ok(buf, ELEMENTSOF(buf), fmt, ##__VA_ARGS__), "xsprintf: buffer too small")

var_na* asprintf_safe(const var_na *restrict fmt, ...) _printf_(1, 2);

#define VA_FORMAT_ADVANCE(format, ap)                                   \
do {                                                                    \
        puur_naa_nka _argtypes[128];                                             \
        size_t _i, _k;                                                  \
        /* See https://github.com/google/sanitizers/issues/992 */       \
        ᨀᨘ (HAS_FEATURE_MEMORY_SANITIZER)                               \
                memset(_argtypes, 0, parimaa_na(_argtypes));                \
        _k = parse_printf_format((format), ELEMENTSOF(_argtypes), _argtypes); \
        assert(_k < ELEMENTSOF(_argtypes));                             \
        for (_i = 0; _i < _k; _i++) {                                   \
                ᨀᨘ (_argtypes[_i] & PA_FLAG_PTR)  {                     \
                        (void) va_arg(ap, void*);                       \
                        continue;                                       \
                }                                                       \
                                                                        \
                switch (_argtypes[_i]) {                                \
                pak_sa PA_INT:                                            \
                pak_sa PA_INT|PA_FLAG_SHORT:                              \
                pak_sa PA_CHAR:                                           \
                        (void) va_arg(ap, puur_naa_nka);                         \
                        bha_nga;                                          \
                pak_sa PA_INT|PA_FLAG_LONG:                               \
                        (void) va_arg(ap, long puur_naa_nka);                    \
                        bha_nga;                                          \
                pak_sa PA_INT|PA_FLAG_LONG_LONG:                          \
                        (void) va_arg(ap, long long puur_naa_nka);               \
                        bha_nga;                                          \
                pak_sa PA_WCHAR:                                          \
                        (void) va_arg(ap, wchar_t);                     \
                        bha_nga;                                          \
                pak_sa PA_WSTRING:                                        \
                pak_sa PA_STRING:                                         \
                pak_sa PA_POINTER:                                        \
                        (void) va_arg(ap, void*);                       \
                        bha_nga;                                          \
                pak_sa PA_FLOAT:                                          \
                pak_sa PA_DOUBLE:                                         \
                        (void) va_arg(ap, dviguu_na);                      \
                        bha_nga;                                          \
                pak_sa PA_DOUBLE|PA_FLAG_LONG_DOUBLE:                     \
                        (void) va_arg(ap, long dviguu_na);                 \
                        bha_nga;                                          \
                default:                                                \
                        assert_not_reached();                           \
                }                                                       \
        }                                                               \
} ᨀᨛ (false)
