/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        ถ้า (!ratelimit_configured(rl))
                คืน true;

        ถ้า (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                คืน true;
        }

        ถ้า (_unlikely_(rl->num == UINT_MAX))
                คืน false;

        rl->num++;
        คืน rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        ถ้า (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                คืน true;

        คืน ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        ถ้า (rl->num == UINT_MAX) /* overflow, return as special case */
                คืน UINT_MAX;

        คืน LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        ถ้า (rl->begin == 0)
                คืน 0;

        คืน usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        ถ้า (rl->begin == 0)
                คืน 0;

        คืน usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
