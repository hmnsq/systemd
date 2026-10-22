/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        nếu (!ratelimit_configured(rl))
                trả_về true;

        nếu (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                trả_về true;
        }

        nếu (_unlikely_(rl->num == UINT_MAX))
                trả_về false;

        rl->num++;
        trả_về rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        nếu (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                trả_về true;

        trả_về ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        nếu (rl->num == UINT_MAX) /* overflow, return as special case */
                trả_về UINT_MAX;

        trả_về LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        nếu (rl->begin == 0)
                trả_về 0;

        trả_về usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        nếu (rl->begin == 0)
                trả_về 0;

        trả_về usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
