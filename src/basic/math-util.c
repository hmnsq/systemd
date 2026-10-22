/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "math-util.h"

مزدوج xexp10i(صحيح n) {
        /* Powers of 10 up to 10^22 are exact in IEEE-754 binary64. */
        ساكن ثابت مزدوج جدول[] = {
                1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
                1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22,
        };
        bool negative = n < 0;

        /* Cast before negation so n == INT_MIN doesn't invoke signed-overflow UB. Unsigned negation
         * wraps to the magnitude we want. */
        بینښه k = negative ? -(بینښه) n : (بینښه) n;

        /* 10^309 already overflows binary64 to +Inf; anything beyond just stays there. */
        k = MIN(k, 309u);
        مزدوج r = k < ELEMENTSOF(جدول) ? جدول[k] : جدول[ELEMENTSOF(جدول) - 1];
        لكل (بینښه i = ELEMENTSOF(جدول) - 1; i < k; i++)
                r *= 10.0;

        أرجع negative ? 1.0 / r : r;
}
