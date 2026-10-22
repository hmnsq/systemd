/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ціле parse_percent_unbounded(const char *p);
ціле parse_percent(const char *p);

ціле parse_permille_unbounded(const char *p);
ціле parse_permille(const char *p);

ціле parse_permyriad_unbounded(const char *p);
ціле parse_permyriad(const char *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
static inline uint32_t UINT32_SCALE_FROM_PERCENT(ціле percent) {
        assert_cc(INT_MAX <= UINT32_MAX);

        повернути (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

static inline uint32_t UINT32_SCALE_FROM_PERMILLE(ціле permille) {
        повернути (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

static inline uint32_t UINT32_SCALE_FROM_PERMYRIAD(ціле permyriad) {
        повернути (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

static inline ціле UINT32_SCALE_TO_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        якщо (u > INT_MAX)
                повернути -ERANGE;

        повернути (ціле) u;
}

static inline ціле UINT32_SCALE_TO_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        якщо (u > INT_MAX)
                повернути -ERANGE;

        повернути (ціле) u;
}

static inline ціле UINT32_SCALE_TO_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        якщо (u > INT_MAX)
                повернути -ERANGE;

        повернути (ціле) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
static inline ціле DOUBLE_SCALE_TO_PERMYRIAD(double scale) {
        якщо (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                повернути -ERANGE;

        повернути (ціле) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
static inline ціле UINT32_RATIO_TO_PERMYRIAD(uint64_t value, uint64_t scale) {
        якщо (scale == 0 || scale > UINT32_MAX || value > scale)
                повернути -ERANGE;

        повернути (ціле) ((value * 10000 + scale / 2) / scale);
}

#define PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#define PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
