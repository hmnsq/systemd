/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

पूर्णाङ्क parse_percent_unbounded(ध्रुव वर्ण *p);
पूर्णाङ्क parse_percent(ध्रुव वर्ण *p);

पूर्णाङ्क parse_permille_unbounded(ध्रुव वर्ण *p);
पूर्णाङ्क parse_permille(ध्रुव वर्ण *p);

पूर्णाङ्क parse_permyriad_unbounded(ध्रुव वर्ण *p);
पूर्णाङ्क parse_permyriad(ध्रुव वर्ण *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
स्थिर अन्तःस्थ uint32_t UINT32_SCALE_FROM_PERCENT(पूर्णाङ्क percent) {
        assert_cc(अधिकतम_पूर्णाङ्कः <= UINT32_MAX);

        प्रत्यागम (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

स्थिर अन्तःस्थ uint32_t UINT32_SCALE_FROM_PERMILLE(पूर्णाङ्क permille) {
        प्रत्यागम (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

स्थिर अन्तःस्थ uint32_t UINT32_SCALE_FROM_PERMYRIAD(पूर्णाङ्क permyriad) {
        प्रत्यागम (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

स्थिर अन्तःस्थ पूर्णाङ्क UINT32_SCALE_गन्तव्यम्_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        यदि (u > अधिकतम_पूर्णाङ्कः)
                प्रत्यागम -ERANGE;

        प्रत्यागम (पूर्णाङ्क) u;
}

स्थिर अन्तःस्थ पूर्णाङ्क UINT32_SCALE_गन्तव्यम्_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        यदि (u > अधिकतम_पूर्णाङ्कः)
                प्रत्यागम -ERANGE;

        प्रत्यागम (पूर्णाङ्क) u;
}

स्थिर अन्तःस्थ पूर्णाङ्क UINT32_SCALE_गन्तव्यम्_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        यदि (u > अधिकतम_पूर्णाङ्कः)
                प्रत्यागम -ERANGE;

        प्रत्यागम (पूर्णाङ्क) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
स्थिर अन्तःस्थ पूर्णाङ्क DOUBLE_SCALE_गन्तव्यम्_PERMYRIAD(द्विगुण scale) {
        यदि (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                प्रत्यागम -ERANGE;

        प्रत्यागम (पूर्णाङ्क) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
स्थिर अन्तःस्थ पूर्णाङ्क UINT32_अनुपातः_गन्तव्यम्_PERMYRIAD(uint64_t मानम्, uint64_t scale) {
        यदि (scale == 0 || scale > UINT32_MAX || मानम् > scale)
                प्रत्यागम -ERANGE;

        प्रत्यागम (पूर्णाङ्क) ((मानम् * 10000 + scale / 2) / scale);
}

#परिभाषय PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#परिभाषय PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
