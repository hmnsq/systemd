/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        प्रतिज्ञा(rl);

        यदि (!ratelimit_configured(rl))
                फर्काऊ true;

        यदि (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->अङ्क = 1;     /* Reset counter */
                फर्काऊ true;
        }

        यदि (_unlikely_(rl->अङ्क == अच_पूर्णांक_उच्च))
                फर्काऊ false;

        rl->अङ्क++;
        फर्काऊ rl->अङ्क <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                फर्काऊ true;

        फर्काऊ ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

अचिन्हित ratelimit_अङ्क_dropped(अचर RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (rl->अङ्क == अच_पूर्णांक_उच्च) /* overflow, return as special case */
                फर्काऊ अच_पूर्णांक_उच्च;

        फर्काऊ LESS_BY(rl->अङ्क, rl->burst);
}

usec_t ratelimit_end(अचर RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (rl->begin == 0)
                फर्काऊ 0;

        फर्काऊ usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_बायाँ(अचर RateLimit *rl) {
        प्रतिज्ञा(rl);

        यदि (rl->begin == 0)
                फर्काऊ 0;

        फर्काऊ usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
