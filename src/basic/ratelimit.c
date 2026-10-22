/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        أكد(rl);

        𐤀𐤌 (!ratelimit_configured(rl))
                أرجع true;

        𐤀𐤌 (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                أرجع true;
        }

        𐤀𐤌 (_unlikely_(rl->num == UINT_MAX))
                أرجع false;

        rl->num++;
        أرجع rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        أكد(rl);

        𐤀𐤌 (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                أرجع true;

        أرجع ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(ثابت RateLimit *rl) {
        أكد(rl);

        𐤀𐤌 (rl->num == UINT_MAX) /* overflow, return as special case */
                أرجع UINT_MAX;

        أرجع LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(ثابت RateLimit *rl) {
        أكد(rl);

        𐤀𐤌 (rl->begin == 0)
                أرجع 0;

        أرجع usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(ثابت RateLimit *rl) {
        أكد(rl);

        𐤀𐤌 (rl->begin == 0)
                أرجع 0;

        أرجع usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
