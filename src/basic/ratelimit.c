/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        ако (!ratelimit_configured(rl))
                врати true;

        ако (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                врати true;
        }

        ако (_unlikely_(rl->num == UINT_MAX))
                врати false;

        rl->num++;
        врати rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        ако (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                врати true;

        врати ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        ако (rl->num == UINT_MAX) /* overflow, return as special case */
                врати UINT_MAX;

        врати LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        ако (rl->begin == 0)
                врати 0;

        врати usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        ако (rl->begin == 0)
                врати 0;

        врати usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
