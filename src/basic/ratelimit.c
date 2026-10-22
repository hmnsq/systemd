/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        если (!ratelimit_configured(rl))
                вернуть true;

        если (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                вернуть true;
        }

        если (_unlikely_(rl->num == UINT_MAX))
                вернуть false;

        rl->num++;
        вернуть rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        если (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                вернуть true;

        вернуть ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        если (rl->num == UINT_MAX) /* overflow, return as special case */
                вернуть UINT_MAX;

        вернуть LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        если (rl->begin == 0)
                вернуть 0;

        вернуть usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        если (rl->begin == 0)
                вернуть 0;

        вернуть usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
