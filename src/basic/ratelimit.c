/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ratelimit.h"
#include "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        assert(rl);

        ប្រសិនបើ (!ratelimit_configured(rl))
                ត្រឡប់ true;

        ប្រសិនបើ (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                ត្រឡប់ true;
        }

        ប្រសិនបើ (_unlikely_(rl->num == UINT_MAX))
                ត្រឡប់ false;

        rl->num++;
        ត្រឡប់ rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        assert(rl);

        ប្រសិនបើ (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                ត្រឡប់ true;

        ត្រឡប់ ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(const RateLimit *rl) {
        assert(rl);

        ប្រសិនបើ (rl->num == UINT_MAX) /* overflow, return as special case */
                ត្រឡប់ UINT_MAX;

        ត្រឡប់ LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(const RateLimit *rl) {
        assert(rl);

        ប្រសិនបើ (rl->begin == 0)
                ត្រឡប់ 0;

        ត្រឡប់ usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(const RateLimit *rl) {
        assert(rl);

        ប្រសិនបើ (rl->begin == 0)
                ត្រឡប់ 0;

        ត្រឡប់ usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
