/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

పూర్ణాంకం parse_percent_unbounded(స్థిరరాశి అక్షరం *p);
పూర్ణాంకం parse_percent(స్థిరరాశి అక్షరం *p);

పూర్ణాంకం parse_permille_unbounded(స్థిరరాశి అక్షరం *p);
పూర్ణాంకం parse_permille(స్థిరరాశి అక్షరం *p);

పూర్ణాంకం parse_permyriad_unbounded(స్థిరరాశి అక్షరం *p);
పూర్ణాంకం parse_permyriad(స్థిరరాశి అక్షరం *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
స్థిర అంతఃవరుస uint32_t UINT32_SCALE_FROM_PERCENT(పూర్ణాంకం percent) {
        assert_cc(గరిష్ఠ_పూర్ణాంకం <= UINT32_MAX);

        తిరిగి (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

స్థిర అంతఃవరుస uint32_t UINT32_SCALE_FROM_PERMILLE(పూర్ణాంకం permille) {
        తిరిగి (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

స్థిర అంతఃవరుస uint32_t UINT32_SCALE_FROM_PERMYRIAD(పూర్ణాంకం permyriad) {
        తిరిగి (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

స్థిర అంతఃవరుస పూర్ణాంకం UINT32_SCALE_గమ్యం_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        ఒకవేళ (u > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -ERANGE;

        తిరిగి (పూర్ణాంకం) u;
}

స్థిర అంతఃవరుస పూర్ణాంకం UINT32_SCALE_గమ్యం_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        ఒకవేళ (u > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -ERANGE;

        తిరిగి (పూర్ణాంకం) u;
}

స్థిర అంతఃవరుస పూర్ణాంకం UINT32_SCALE_గమ్యం_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        ఒకవేళ (u > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -ERANGE;

        తిరిగి (పూర్ణాంకం) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
స్థిర అంతఃవరుస పూర్ణాంకం DOUBLE_SCALE_గమ్యం_PERMYRIAD(ద్విగుణ scale) {
        ఒకవేళ (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                తిరిగి -ERANGE;

        తిరిగి (పూర్ణాంకం) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
స్థిర అంతఃవరుస పూర్ణాంకం UINT32_నిష్పత్తి_గమ్యం_PERMYRIAD(uint64_t విలువ, uint64_t scale) {
        ఒకవేళ (scale == 0 || scale > UINT32_MAX || విలువ > scale)
                తిరిగి -ERANGE;

        తిరిగి (పూర్ణాంకం) ((విలువ * 10000 + scale / 2) / scale);
}

#నిర్వచించు PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#నిర్వచించు PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
