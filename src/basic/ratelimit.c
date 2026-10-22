/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "ratelimit.h"
#અન્તર્ભાવ "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        પ્રતિજ્ઞા(rl);

        જો (!ratelimit_configured(rl))
                પાછા true;

        જો (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->અંક = 1;     /* Reset counter */
                પાછા true;
        }

        જો (_unlikely_(rl->અંક == UINT_MAX))
                પાછા false;

        rl->અંક++;
        પાછા rl->અંક <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        પ્રતિજ્ઞા(rl);

        જો (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                પાછા true;

        પાછા ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_અંક_dropped(અચલ RateLimit *rl) {
        પ્રતિજ્ઞા(rl);

        જો (rl->અંક == UINT_MAX) /* overflow, return as special case */
                પાછા UINT_MAX;

        પાછા LESS_BY(rl->અંક, rl->burst);
}

usec_t ratelimit_end(અચલ RateLimit *rl) {
        પ્રતિજ્ઞા(rl);

        જો (rl->begin == 0)
                પાછા 0;

        પાછા usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_ડાબું(અચલ RateLimit *rl) {
        પ્રતિજ્ઞા(rl);

        જો (rl->begin == 0)
                પાછા 0;

        પાછા usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
