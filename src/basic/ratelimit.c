/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        أكد(rl);

        ܐܢ (!ratelimit_configured(rl))
                ܦܢܐ true;

        ܐܢ (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->num = 1;     /* Reset counter */
                ܦܢܐ true;
        }

        ܐܢ (_unlikely_(rl->num == UINT_MAX))
                ܦܢܐ false;

        rl->num++;
        ܦܢܐ rl->num <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        أكد(rl);

        ܐܢ (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                ܦܢܐ true;

        ܦܢܐ ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_num_dropped(ثابت RateLimit *rl) {
        أكد(rl);

        ܐܢ (rl->num == UINT_MAX) /* overflow, return as special case */
                ܦܢܐ UINT_MAX;

        ܦܢܐ LESS_BY(rl->num, rl->burst);
}

usec_t ratelimit_end(ثابت RateLimit *rl) {
        أكد(rl);

        ܐܢ (rl->begin == 0)
                ܦܢܐ 0;

        ܦܢܐ usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_left(ثابت RateLimit *rl) {
        أكد(rl);

        ܐܢ (rl->begin == 0)
                ܦܢܐ 0;

        ܦܢܐ usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
