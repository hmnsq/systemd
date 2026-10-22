/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

صحیح parse_percent_unbounded(ثابت نویسه *p);
صحیح parse_percent(ثابت نویسه *p);

صحیح parse_permille_unbounded(ثابت نویسه *p);
صحیح parse_permille(ثابت نویسه *p);

صحیح parse_permyriad_unbounded(ثابت نویسه *p);
صحیح parse_permyriad(ثابت نویسه *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
ایستا درونخطی uint32_t UINT32_SCALE_FROM_PERCENT(صحیح percent) {
        assert_cc(INT_MAX <= UINT32_MAX);

        بازگشت (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

ایستا درونخطی uint32_t UINT32_SCALE_FROM_PERMILLE(صحیح permille) {
        بازگشت (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

ایستا درونخطی uint32_t UINT32_SCALE_FROM_PERMYRIAD(صحیح permyriad) {
        بازگشت (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

ایستا درونخطی صحیح UINT32_SCALE_TO_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        اگر (u > INT_MAX)
                بازگشت -ERANGE;

        بازگشت (صحیح) u;
}

ایستا درونخطی صحیح UINT32_SCALE_TO_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        اگر (u > INT_MAX)
                بازگشت -ERANGE;

        بازگشت (صحیح) u;
}

ایستا درونخطی صحیح UINT32_SCALE_TO_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        اگر (u > INT_MAX)
                بازگشت -ERANGE;

        بازگشت (صحیح) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
ایستا درونخطی صحیح DOUBLE_SCALE_TO_PERMYRIAD(دوگانه scale) {
        اگر (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                بازگشت -ERANGE;

        بازگشت (صحیح) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
ایستا درونخطی صحیح UINT32_RATIO_TO_PERMYRIAD(uint64_t مقدار, uint64_t scale) {
        اگر (scale == 0 || scale > UINT32_MAX || مقدار > scale)
                بازگشت -ERANGE;

        بازگشت (صحیح) ((مقدار * 10000 + scale / 2) / scale);
}

#define PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#define PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
