/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "math-util.h"

ਦੁੱਗਣਾ xexp10i(ਪੂਰਨ n) {
        /* Powers of 10 up to 10^22 are exact in IEEE-754 binary64. */
        ਸਥਿਰ ਅਚਲ ਦੁੱਗਣਾ ਸਾਰਣੀ[] = {
                1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
                1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22,
        };
        bool negative = n < 0;

        /* Cast before negation so n == INT_MIN doesn't invoke signed-overflow UB. Unsigned negation
         * wraps to the magnitude we want. */
        ਬਿਨਾਨਿਸ਼ਾਨੀ k = negative ? -(ਬਿਨਾਨਿਸ਼ਾਨੀ) n : (ਬਿਨਾਨਿਸ਼ਾਨੀ) n;

        /* 10^309 already overflows binary64 to +Inf; anything beyond just stays there. */
        k = MIN(k, 309u);
        ਦੁੱਗਣਾ r = k < ELEMENTSOF(ਸਾਰਣੀ) ? ਸਾਰਣੀ[k] : ਸਾਰਣੀ[ELEMENTSOF(ਸਾਰਣੀ) - 1];
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = ELEMENTSOF(ਸਾਰਣੀ) - 1; i < k; i++)
                r *= 10.0;

        ਵਾਪਸ negative ? 1.0 / r : r;
}
