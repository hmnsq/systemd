/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        प्रतिज्ञा(rl);

        सचे (!ratelimit_configured(rl))
                पटिनिवत्त true;

        सचे (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->अङ्कः = 1;     /* Reset counter */
                पटिनिवत्त true;
        }

        सचे (_unlikely_(rl->अङ्कः == UINT_MAX))
                पटिनिवत्त false;

        rl->अङ्कः++;
        पटिनिवत्त rl->अङ्कः <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        प्रतिज्ञा(rl);

        सचे (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                पटिनिवत्त true;

        पटिनिवत्त ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_अङ्कः_dropped(ध्रुव RateLimit *rl) {
        प्रतिज्ञा(rl);

        सचे (rl->अङ्कः == UINT_MAX) /* overflow, return as special case */
                पटिनिवत्त UINT_MAX;

        पटिनिवत्त LESS_BY(rl->अङ्कः, rl->burst);
}

usec_t ratelimit_end(ध्रुव RateLimit *rl) {
        प्रतिज्ञा(rl);

        सचे (rl->begin == 0)
                पटिनिवत्त 0;

        पटिनिवत्त usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_वामः(ध्रुव RateLimit *rl) {
        प्रतिज्ञा(rl);

        सचे (rl->begin == 0)
                पटिनिवत्त 0;

        पटिनिवत्त usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
