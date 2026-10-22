/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "ratelimit.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        ପ୍ରତିଜ୍ଞା(rl);

        ଯଦି (!ratelimit_configured(rl))
                ଫେରନ୍ତୁ true;

        ଯଦି (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->ଅଙ୍କ = 1;     /* Reset counter */
                ଫେରନ୍ତୁ true;
        }

        ଯଦି (_unlikely_(rl->ଅଙ୍କ == UINT_MAX))
                ଫେରନ୍ତୁ false;

        rl->ଅଙ୍କ++;
        ଫେରନ୍ତୁ rl->ଅଙ୍କ <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        ପ୍ରତିଜ୍ଞା(rl);

        ଯଦି (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_ଅଙ୍କ_dropped(ଅଚଳ RateLimit *rl) {
        ପ୍ରତିଜ୍ଞା(rl);

        ଯଦି (rl->ଅଙ୍କ == UINT_MAX) /* overflow, return as special case */
                ଫେରନ୍ତୁ UINT_MAX;

        ଫେରନ୍ତୁ LESS_BY(rl->ଅଙ୍କ, rl->burst);
}

usec_t ratelimit_end(ଅଚଳ RateLimit *rl) {
        ପ୍ରତିଜ୍ଞା(rl);

        ଯଦି (rl->begin == 0)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_ବାମ(ଅଚଳ RateLimit *rl) {
        ପ୍ରତିଜ୍ଞା(rl);

        ଯଦି (rl->begin == 0)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
