/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ parse_percent_unbounded(ਅਚਲ ਅੱਖਰ *p);
ਪੂਰਨ parse_percent(ਅਚਲ ਅੱਖਰ *p);

ਪੂਰਨ parse_permille_unbounded(ਅਚਲ ਅੱਖਰ *p);
ਪੂਰਨ parse_permille(ਅਚਲ ਅੱਖਰ *p);

ਪੂਰਨ parse_permyriad_unbounded(ਅਚਲ ਅੱਖਰ *p);
ਪੂਰਨ parse_permyriad(ਅਚਲ ਅੱਖਰ *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t UINT32_SCALE_FROM_PERCENT(ਪੂਰਨ percent) {
        assert_cc(ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ <= UINT32_MAX);

        ਵਾਪਸ (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t UINT32_SCALE_FROM_PERMILLE(ਪੂਰਨ permille) {
        ਵਾਪਸ (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t UINT32_SCALE_FROM_PERMYRIAD(ਪੂਰਨ permyriad) {
        ਵਾਪਸ (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ UINT32_SCALE_ਮੰਜ਼ਿਲ_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        ਜੇ (u > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ (ਪੂਰਨ) u;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ UINT32_SCALE_ਮੰਜ਼ਿਲ_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        ਜੇ (u > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ (ਪੂਰਨ) u;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ UINT32_SCALE_ਮੰਜ਼ਿਲ_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        ਜੇ (u > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ (ਪੂਰਨ) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ DOUBLE_SCALE_ਮੰਜ਼ਿਲ_PERMYRIAD(ਦੁੱਗਣਾ scale) {
        ਜੇ (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ (ਪੂਰਨ) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ UINT32_ਅਨੁਪਾਤ_ਮੰਜ਼ਿਲ_PERMYRIAD(uint64_t ਮੁੱਲ, uint64_t scale) {
        ਜੇ (scale == 0 || scale > UINT32_MAX || ਮੁੱਲ > scale)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ (ਪੂਰਨ) ((ਮੁੱਲ * 10000 + scale / 2) / scale);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
