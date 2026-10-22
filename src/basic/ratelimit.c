/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        أكد(rl);

        إذا (!ratelimit_configured(rl))
                إرجاع true;

        إذا (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->رقم = 1;     /* Reset counter */
                إرجاع true;
        }

        إذا (_unlikely_(rl->رقم == UINT_MAX))
                إرجاع false;

        rl->رقم++;
        إرجاع rl->رقم <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        أكد(rl);

        إذا (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                إرجاع true;

        إرجاع ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_رقم_dropped(ثابت RateLimit *rl) {
        أكد(rl);

        إذا (rl->رقم == UINT_MAX) /* overflow, return as special case */
                إرجاع UINT_MAX;

        إرجاع LESS_BY(rl->رقم, rl->burst);
}

usec_t ratelimit_end(ثابت RateLimit *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                إرجاع 0;

        إرجاع usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_يسار(ثابت RateLimit *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                إرجاع 0;

        إرجاع usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
