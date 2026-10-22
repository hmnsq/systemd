/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        якщо (!ratelimit_configured(rl))
                повернути true;

        якщо (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                повернути true;
        }

        якщо (_unlikely_(rl->num == UINT_MAX))
                повернути false;

        rl->num++;
        повернути rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        якщо (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                повернути true;

        повернути ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        якщо (rl->num == UINT_MAX) /* overflow, return as special case */
                повернути UINT_MAX;

        повернути LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        якщо (rl->begin == 0)
                повернути 0;

        повернути usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        якщо (rl->begin == 0)
                повернути 0;

        повернути usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
