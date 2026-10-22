/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        أكد(rl);

        إذا (!ratelimit_configured(rl))
                أرجع true;

        إذا (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->لمب = 1;     /* Reset counter */
                أرجع true;
        }

        إذا (_unlikely_(rl->لمب == UINT_MAX))
                أرجع false;

        rl->لمب++;
        أرجع rl->لمب <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        أكد(rl);

        إذا (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                أرجع true;

        أرجع ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_لمب_dropped(ثابت RateLimit *rl) {
        أكد(rl);

        إذا (rl->لمب == UINT_MAX) /* overflow, return as special case */
                أرجع UINT_MAX;

        أرجع LESS_BY(rl->لمب, rl->burst);
}

usec_t ratelimit_end(ثابت RateLimit *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_هغو(ثابت RateLimit *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
