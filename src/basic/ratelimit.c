/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "ratelimit.h"
#समावेश "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        निश्चित(rl);

        अगर (!ratelimit_configured(rl))
                वापस true;

        अगर (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->अंक = 1;     /* Reset counter */
                वापस true;
        }

        अगर (_unlikely_(rl->अंक == अच_पूर्णांक_उच्च))
                वापस false;

        rl->अंक++;
        वापस rl->अंक <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        निश्चित(rl);

        अगर (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                वापस true;

        वापस ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

अचिन्हित ratelimit_अंक_dropped(स्थिर RateLimit *rl) {
        निश्चित(rl);

        अगर (rl->अंक == अच_पूर्णांक_उच्च) /* overflow, return as special case */
                वापस अच_पूर्णांक_उच्च;

        वापस LESS_BY(rl->अंक, rl->burst);
}

usec_t ratelimit_end(स्थिर RateLimit *rl) {
        निश्चित(rl);

        अगर (rl->begin == 0)
                वापस 0;

        वापस usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_बायाँ(स्थिर RateLimit *rl) {
        निश्चित(rl);

        अगर (rl->begin == 0)
                वापस 0;

        वापस usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
