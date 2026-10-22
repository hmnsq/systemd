/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "ratelimit.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_below_at(RateLimit *rl, usec_t ts) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಯದಿ (!ratelimit_configured(rl))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->ಅಂಕೆ = 1;     /* Reset counter */
                ಪ್ರತ್ಯಾಗಮ true;
        }

        ಯದಿ (_unlikely_(rl->ಅಂಕೆ == UINT_MAX))
                ಪ್ರತ್ಯಾಗಮ false;

        rl->ಅಂಕೆ++;
        ಪ್ರತ್ಯಾಗಮ rl->ಅಂಕೆ <= rl->burst;
}

bool ratelimit_below(RateLimit *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಯದಿ (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ ratelimit_below_at(rl, now(CLOCK_BOOTTIME));
}

unsigned ratelimit_ಅಂಕೆ_dropped(ಧ್ರುವ RateLimit *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಯದಿ (rl->ಅಂಕೆ == UINT_MAX) /* overflow, return as special case */
                ಪ್ರತ್ಯಾಗಮ UINT_MAX;

        ಪ್ರತ್ಯಾಗಮ LESS_BY(rl->ಅಂಕೆ, rl->burst);
}

usec_t ratelimit_end(ಧ್ರುವ RateLimit *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಯದಿ (rl->begin == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ usec_add(rl->begin, rl->interval);
}

usec_t ratelimit_ಎಡ(ಧ್ರುವ RateLimit *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಯದಿ (rl->begin == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ usec_sub_unsigned(ratelimit_end(rl), now(CLOCK_BOOTTIME));
}
