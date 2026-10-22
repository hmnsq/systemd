/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <math.h>       /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

/* On some optimization level, iszero(x) is converted to (x == 0.0), and emits warning -Wfloat-equal.
 * The argument must be a floating point, i.e. one of float, double, or long double. */
#ವ್ಯಾಖ್ಯಾನಿಸು iszero_ಸುರಕ್ಷಿತ(x) (fpclassify(x) == FP_ZERO)

/* To avoid x == y and triggering compile warning -Wfloat-equal. This returns false if one of the argument is
 * NaN or infinity. One of the argument must be a floating point.
 *
 * The volatile temporaries force a memory roundtrip, truncating any excess precision (e.g. x87's
 * 80-bit register width for double arithmetic) down to the declared type. -fexcess-precision=standard
 * doesn't fully cover this on x87 — a function return value carried in ST(0) can still arrive at the
 * caller in 80-bit precision (see gcc PR#323), so a value that should compare equal to a
 * same-magnitude literal picks up extra mantissa bits and doesn't. The memory store-and-reload is
 * the one operation guaranteed to truncate. The temporaries inherit the type of the subtraction
 * expression so the macro stays generic over float / double / long double rather than silently
 * truncating wider arguments. */
#ವ್ಯಾಖ್ಯಾನಿಸು fp_equal(x, y)                                                  \
        ({                                                              \
                ಅಸ್ಥಿರ __typeof__((x) - (y)) _fp_x = (x);             \
                ಅಸ್ಥಿರ __typeof__((x) - (y)) _fp_y = (y);             \
                iszero_ಸುರಕ್ಷಿತ(_fp_x - _fp_y);                             \
        })

/* 10^n. Exact for |n| ≤ 22; otherwise multiplies and may accumulate rounding error. Saturates to
 * 0.0 or +Inf outside binary64's exponent range; large |n| is capped internally so untrusted
 * inputs can't cause unbounded work. */
ದ್ವಿಗುಣ xexp10i(ಪೂರ್ಣಾಂಕ n);
