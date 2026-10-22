/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        如果 (!ratelimit_configured(rl))
                返回 true;

        如果 (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                返回 true;
        }

        如果 (_unlikely_(rl->num == UINT_MAX))
                返回 false;

        rl->num++;
        返回 rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        如果 (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                返回 true;

        返回 ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        如果 (rl->num == UINT_MAX) /* overflow, return as special case */
                返回 UINT_MAX;

        返回 LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        如果 (rl->begin == 0)
                返回 0;

        返回 usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        如果 (rl->begin == 0)
                返回 0;

        返回 usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
