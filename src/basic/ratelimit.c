/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "ratelimit.h"
#অন্তর্ভাব "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        প্রতিজ্ঞা(rl);

        যদি (!ratelimit_configured(rl))
                প্রত্যাগম true;

        যদি (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->অঙ্ক = 1;     /* Reset counter */
                প্রত্যাগম true;
        }

        যদি (_unlikely_(rl->অঙ্ক == UINT_MAX))
                প্রত্যাগম false;

        rl->অঙ্ক++;
        প্রত্যাগম rl->অঙ্ক <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        প্রতিজ্ঞা(rl);

        যদি (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                প্রত্যাগম true;

        প্রত্যাগম ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_অঙ্ক_dropped(ধ্রুব RateLimit *rl) {
        প্রতিজ্ঞা(rl);

        যদি (rl->অঙ্ক == UINT_MAX) /* overflow, return as special case */
                প্রত্যাগম UINT_MAX;

        প্রত্যাগম LESS_BY(rl->অঙ্ক, rl->burst);
}

usec_t ratelimit_end(ধ্রুব RateLimit *rl) {
        প্রতিজ্ঞা(rl);

        যদি (rl->begin == 0)
                প্রত্যাগম 0;

        প্রত্যাগম usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_বাম(ধ্রুব RateLimit *rl) {
        প্রতিজ্ঞা(rl);

        যদি (rl->begin == 0)
                প্রত্যাগম 0;

        প্রত্যাগম usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
