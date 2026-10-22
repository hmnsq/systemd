/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

صحيح parse_percent_unbounded(ثابت حرف *p);
صحيح parse_percent(ثابت حرف *p);

صحيح parse_permille_unbounded(ثابت حرف *p);
صحيح parse_permille(ثابت حرف *p);

صحيح parse_permyriad_unbounded(ثابت حرف *p);
صحيح parse_permyriad(ثابت حرف *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
ساكن مضمن uint32_t UINT32_SCALE_FROM_PERCENT(صحيح percent) {
        assert_cc(INT_MAX <= UINT32_MAX);

        أرجع (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

ساكن مضمن uint32_t UINT32_SCALE_FROM_PERMILLE(صحيح permille) {
        أرجع (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

ساكن مضمن uint32_t UINT32_SCALE_FROM_PERMYRIAD(صحيح permyriad) {
        أرجع (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

ساكن مضمن صحيح UINT32_SCALE_TO_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        إذا (u > INT_MAX)
                أرجع -ERANGE;

        أرجع (صحيح) u;
}

ساكن مضمن صحيح UINT32_SCALE_TO_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        إذا (u > INT_MAX)
                أرجع -ERANGE;

        أرجع (صحيح) u;
}

ساكن مضمن صحيح UINT32_SCALE_TO_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        إذا (u > INT_MAX)
                أرجع -ERANGE;

        أرجع (صحيح) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
ساكن مضمن صحيح DOUBLE_SCALE_TO_PERMYRIAD(مزدوج scale) {
        إذا (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                أرجع -ERANGE;

        أرجع (صحيح) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
ساكن مضمن صحيح UINT32_RATIO_TO_PERMYRIAD(uint64_t نيلاي, uint64_t scale) {
        إذا (scale == 0 || scale > UINT32_MAX || نيلاي > scale)
                أرجع -ERANGE;

        أرجع (صحيح) ((نيلاي * 10000 + scale / 2) / scale);
}

#define PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#define PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
