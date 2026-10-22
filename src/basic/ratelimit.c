/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        प्रतिज्ञा(rl);

        यदि (!ratelimit_configured(rl))
                प्रत्यागम true;

        यदि (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->अंक = 1;     /* Reset counter */
                प्रत्यागम true;
        }

        यदि (_unlikely_(rl->अंक == अच_पूर्णांक_उच्च))
                प्रत्यागम false;

        rl->अंक++;
        प्रत्यागम rl->अंक <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                प्रत्यागम true;

        प्रत्यागम ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

अचिन्हित ratelimit_अंक_dropped(ध्रुव RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (rl->अंक == अच_पूर्णांक_उच्च) /* overflow, return as special case */
                प्रत्यागम अच_पूर्णांक_उच्च;

        प्रत्यागम LESS_BY(rl->अंक, rl->burst);
}

usec_t ratelimit_end(ध्रुव RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (rl->begin == 0)
                प्रत्यागम 0;

        प्रत्यागम usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_बायाँ(ध्रुव RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (rl->begin == 0)
                प्रत्यागम 0;

        प्रत्यागम usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
