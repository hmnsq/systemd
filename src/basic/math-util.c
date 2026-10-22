/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "math-util.h"

ద్విగుణ xexp10i(పూర్ణాంకం n) {
        /* Powers of 10 up to 10^22 are exact in IEEE-754 binary64. */
        స్థిర స్థిరరాశి ద్విగుణ పట్టిక[] = {
                1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
                1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22,
        };
        bool negative = n < 0;

        /* Cast before negation so n == INT_MIN doesn't invoke signed-overflow UB. Unsigned negation
         * wraps to the magnitude we want. */
        గుర్తులేని k = negative ? -(గుర్తులేని) n : (గుర్తులేని) n;

        /* 10^309 already overflows binary64 to +Inf; anything beyond just stays there. */
        k = MIN(k, 309u);
        ద్విగుణ r = k < ELEMENTSOF(పట్టిక) ? పట్టిక[k] : పట్టిక[ELEMENTSOF(పట్టిక) - 1];
        కొరకు (గుర్తులేని i = ELEMENTSOF(పట్టిక) - 1; i < k; i++)
                r *= 10.0;

        తిరిగి negative ? 1.0 / r : r;
}
