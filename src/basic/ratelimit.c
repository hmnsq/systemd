/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        प्रतिज्ञा(rl);

        जर (!ratelimit_configured(rl))
                परत true;

        जर (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->अंक = 1;     /* Reset counter */
                परत true;
        }

        जर (_unlikely_(rl->अंक == अच_पूर्णांक_उच्च))
                परत false;

        rl->अंक++;
        परत rl->अंक <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        प्रतिज्ञा(rl);

        जर (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                परत true;

        परत ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

अचिन्हित ratelimit_अंक_dropped(स्थिरांक RateLimit *rl) {
        प्रतिज्ञा(rl);

        जर (rl->अंक == अच_पूर्णांक_उच्च) /* overflow, return as special case */
                परत अच_पूर्णांक_उच्च;

        परत LESS_BY(rl->अंक, rl->burst);
}

usec_t ratelimit_end(स्थिरांक RateLimit *rl) {
        प्रतिज्ञा(rl);

        जर (rl->begin == 0)
                परत 0;

        परत usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_डावा(स्थिरांक RateLimit *rl) {
        प्रतिज्ञा(rl);

        जर (rl->begin == 0)
                परत 0;

        परत usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
