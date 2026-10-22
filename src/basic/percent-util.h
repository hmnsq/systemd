/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা parse_percent_unbounded(ধ্রুবক অক্ষর *p);
পূর্ণসংখ্যা parse_percent(ধ্রুবক অক্ষর *p);

পূর্ণসংখ্যা parse_permille_unbounded(ধ্রুবক অক্ষর *p);
পূর্ণসংখ্যা parse_permille(ধ্রুবক অক্ষর *p);

পূর্ণসংখ্যা parse_permyriad_unbounded(ধ্রুবক অক্ষর *p);
পূর্ণসংখ্যা parse_permyriad(ধ্রুবক অক্ষর *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
স্থির অন্তঃরেখ uint32_t UINT32_SCALE_FROM_PERCENT(পূর্ণসংখ্যা percent) {
        assert_cc(INT_MAX <= UINT32_MAX);

        ফেরত (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

স্থির অন্তঃরেখ uint32_t UINT32_SCALE_FROM_PERMILLE(পূর্ণসংখ্যা permille) {
        ফেরত (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

স্থির অন্তঃরেখ uint32_t UINT32_SCALE_FROM_PERMYRIAD(পূর্ণসংখ্যা permyriad) {
        ফেরত (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা UINT32_SCALE_TO_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        যদি (u > INT_MAX)
                ফেরত -ERANGE;

        ফেরত (পূর্ণসংখ্যা) u;
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা UINT32_SCALE_TO_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        যদি (u > INT_MAX)
                ফেরত -ERANGE;

        ফেরত (পূর্ণসংখ্যা) u;
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা UINT32_SCALE_TO_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        যদি (u > INT_MAX)
                ফেরত -ERANGE;

        ফেরত (পূর্ণসংখ্যা) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
স্থির অন্তঃরেখ পূর্ণসংখ্যা DOUBLE_SCALE_TO_PERMYRIAD(দ্বিগুণ scale) {
        যদি (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                ফেরত -ERANGE;

        ফেরত (পূর্ণসংখ্যা) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
স্থির অন্তঃরেখ পূর্ণসংখ্যা UINT32_RATIO_TO_PERMYRIAD(uint64_t মান, uint64_t scale) {
        যদি (scale == 0 || scale > UINT32_MAX || মান > scale)
                ফেরত -ERANGE;

        ফেরত (পূর্ণসংখ্যা) ((মান * 10000 + scale / 2) / scale);
}

#define PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#define PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
