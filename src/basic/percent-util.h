/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ parse_percent_unbounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
ಪೂರ್ಣಾಂಕ parse_percent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);

ಪೂರ್ಣಾಂಕ parse_permille_unbounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
ಪೂರ್ಣಾಂಕ parse_permille(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);

ಪೂರ್ಣಾಂಕ parse_permyriad_unbounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
ಪೂರ್ಣಾಂಕ parse_permyriad(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t UINT32_SCALE_FROM_PERCENT(ಪೂರ್ಣಾಂಕ percent) {
        assert_cc(ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ <= UINT32_MAX);

        ಹಿಂತಿರುಗಿ (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t UINT32_SCALE_FROM_PERMILLE(ಪೂರ್ಣಾಂಕ permille) {
        ಹಿಂತಿರುಗಿ (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t UINT32_SCALE_FROM_PERMYRIAD(ಪೂರ್ಣಾಂಕ permyriad) {
        ಹಿಂತಿರುಗಿ (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ UINT32_SCALE_ಗಮ್ಯ_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        ಆದರೆ (u > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) u;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ UINT32_SCALE_ಗಮ್ಯ_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        ಆದರೆ (u > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) u;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ UINT32_SCALE_ಗಮ್ಯ_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        ಆದರೆ (u > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ DOUBLE_SCALE_ಗಮ್ಯ_PERMYRIAD(ದ್ವಿಗುಣ scale) {
        ಆದರೆ (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ UINT32_ಅನುಪಾತ_ಗಮ್ಯ_PERMYRIAD(uint64_t ಮೌಲ್ಯ, uint64_t scale) {
        ಆದರೆ (scale == 0 || scale > UINT32_MAX || ಮೌಲ್ಯ > scale)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) ((ಮೌಲ್ಯ * 10000 + scale / 2) / scale);
}

#ವ್ಯಾಖ್ಯಾನಿಸು PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#ವ್ಯಾಖ್ಯಾನಿಸು PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
