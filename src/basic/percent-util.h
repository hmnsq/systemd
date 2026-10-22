/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ parse_percent_unbounded(സ്ഥിരം അക്ഷരം *p);
പൂർണ്ണസംഖ്യ parse_percent(സ്ഥിരം അക്ഷരം *p);

പൂർണ്ണസംഖ്യ parse_permille_unbounded(സ്ഥിരം അക്ഷരം *p);
പൂർണ്ണസംഖ്യ parse_permille(സ്ഥിരം അക്ഷരം *p);

പൂർണ്ണസംഖ്യ parse_permyriad_unbounded(സ്ഥിരം അക്ഷരം *p);
പൂർണ്ണസംഖ്യ parse_permyriad(സ്ഥിരം അക്ഷരം *p);

/* Some macro-like helpers that convert a percent/permille/permyriad value (as parsed by parse_percent()) to
 * a value relative to 100% == 2^32-1. Rounds to closest. */
സ്ഥിതം വരിയിൽ uint32_t UINT32_SCALE_FROM_PERCENT(പൂർണ്ണസംഖ്യ percent) {
        assert_cc(INT_MAX <= UINT32_MAX);

        തിരികെ (uint32_t) (((uint64_t) CLAMP(percent, 0, 100) * UINT32_MAX + 50) / 100U);
}

സ്ഥിതം വരിയിൽ uint32_t UINT32_SCALE_FROM_PERMILLE(പൂർണ്ണസംഖ്യ permille) {
        തിരികെ (uint32_t) (((uint64_t) CLAMP(permille, 0, 1000) * UINT32_MAX + 500) / 1000U);
}

സ്ഥിതം വരിയിൽ uint32_t UINT32_SCALE_FROM_PERMYRIAD(പൂർണ്ണസംഖ്യ permyriad) {
        തിരികെ (uint32_t) (((uint64_t) CLAMP(permyriad, 0, 10000) * UINT32_MAX + 5000) / 10000U);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ UINT32_SCALE_TO_PERCENT(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 100U + UINT32_MAX/2) / UINT32_MAX);
        എങ്കിൽ (u > INT_MAX)
                തിരികെ -ERANGE;

        തിരികെ (പൂർണ്ണസംഖ്യ) u;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ UINT32_SCALE_TO_PERMILLE(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 1000U + UINT32_MAX/2) / UINT32_MAX);
        എങ്കിൽ (u > INT_MAX)
                തിരികെ -ERANGE;

        തിരികെ (പൂർണ്ണസംഖ്യ) u;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ UINT32_SCALE_TO_PERMYRIAD(uint32_t scale) {
        uint32_t u;

        u = (uint32_t) ((((uint64_t) scale) * 10000U + UINT32_MAX/2) / UINT32_MAX);
        എങ്കിൽ (u > INT_MAX)
                തിരികെ -ERANGE;

        തിരികെ (പൂർണ്ണസംഖ്യ) u;
}

/* Converts a fraction normalized so that 1.0 == 100% into permyriad. Returns -ERANGE for values outside
 * the 0.0…1.0 range, including NaN. */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ DOUBLE_SCALE_TO_PERMYRIAD(ഇരട്ട scale) {
        എങ്കിൽ (!(scale >= 0.0 && scale <= 1.0)) /* also catches NaN */
                തിരികെ -ERANGE;

        തിരികെ (പൂർണ്ണസംഖ്യ) (scale * 10000.0 + 0.5);
}

/* Converts the ratio value/scale into permyriad. The scale must be no more than UINT32_MAX, which keeps
 * the multiplication inside uint64_t. Returns -ERANGE if the scale is zero or above UINT32_MAX, or if
 * the value is above the scale. */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ UINT32_RATIO_TO_PERMYRIAD(uint64_t മൂല്യം, uint64_t scale) {
        എങ്കിൽ (scale == 0 || scale > UINT32_MAX || മൂല്യം > scale)
                തിരികെ -ERANGE;

        തിരികെ (പൂർണ്ണസംഖ്യ) ((മൂല്യം * 10000 + scale / 2) / scale);
}

#define PERMYRIAD_AS_PERCENT_FORMAT_STR "%i.%02i%%"
#define PERMYRIAD_AS_PERCENT_FORMAT_VAL(x) ((x)/100), ((x)%100)
