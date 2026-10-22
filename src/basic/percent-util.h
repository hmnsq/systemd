/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક parse_percent_unbounded(અચલ અક્ષર *p);
પૂર્ણાંક parse_percent(અચલ અક્ષર *p);

પૂર્ણાંક parse_permille_unbounded(અચલ અક્ષર *p);
પૂર્ણાંક parse_permille(અચલ અક્ષર *p);

પૂર્ણાંક parse_permyriad_unbounded(અચલ અક્ષર *p);
પૂર્ણાંક parse_permyriad(અચલ અક્ષર *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
સ્થિર અંતઃરેખ uint32_t UINT32_SCALE_FROM_PERCENT(પૂર્ણાંક percent) {
        assert_cc(INT_MAX <= UINT32_MAX);

        પાછા (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

સ્થિર અંતઃરેખ uint32_t UINT32_SCALE_FROM_PERMILLE(પૂર્ણાંક permille) {
        પાછા (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

સ્થિર અંતઃરેખ uint32_t UINT32_SCALE_FROM_PERMYRIAD(પૂર્ણાંક permyriad) {
        પાછા (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

સ્થિર અંતઃરેખ પૂર્ણાંક UINT32_SCALE_TO_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        જો (u > INT_MAX)
                પાછા -ERANGE;

        પાછા (પૂર્ણાંક) u;
}

સ્થિર અંતઃરેખ પૂર્ણાંક UINT32_SCALE_TO_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        જો (u > INT_MAX)
                પાછા -ERANGE;

        પાછા (પૂર્ણાંક) u;
}

સ્થિર અંતઃરેખ પૂર્ણાંક UINT32_SCALE_TO_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        જો (u > INT_MAX)
                પાછા -ERANGE;

        પાછા (પૂર્ણાંક) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
સ્થિર અંતઃરેખ પૂર્ણાંક DOUBLE_SCALE_TO_PERMYRIAD(દ્વિગુણ scale) {
        જો (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                પાછા -ERANGE;

        પાછા (પૂર્ણાંક) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
સ્થિર અંતઃરેખ પૂર્ણાંક UINT32_RATIO_TO_PERMYRIAD(uint64_t કિંમત, uint64_t scale) {
        જો (scale == 0 || scale > UINT32_MAX || કિંમત > scale)
                પાછા -ERANGE;

        પાછા (પૂર્ણાંક) ((કિંમત * 10000 + scale / 2) / scale);
}

#define PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#define PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
