/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "ratelimit.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_ಕೆಳಗೆ_at(ದರ_ಎಲ್ಲೆ *rl, usec_t ts) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಆದರೆ (!ratelimit_configured(rl))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->ಅಂಕೆ = 1;     /* Reset counter */
                ಹಿಂತಿರುಗಿ true;
        }

        ಆದರೆ (_unlikely_(rl->ಅಂಕೆ == UINT_MAX))
                ಹಿಂತಿರುಗಿ false;

        rl->ಅಂಕೆ++;
        ಹಿಂತಿರುಗಿ rl->ಅಂಕೆ <= rl->ಅವಧಿ;
}

bool ratelimit_ಕೆಳಗೆ(ದರ_ಎಲ್ಲೆ *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಆದರೆ (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ ratelimit_ಕೆಳಗೆ_at(rl, now(CLOCK_BOOTTIME));
}

ಚಿಹ್ನೆರಹಿತ ratelimit_ಅಂಕೆ_dropped(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಆದರೆ (rl->ಅಂಕೆ == UINT_MAX) /* overflow, return as special case */
                ಹಿಂತಿರುಗಿ UINT_MAX;

        ಹಿಂತಿರುಗಿ LESS_BY(rl->ಅಂಕೆ, rl->ಅವಧಿ);
}

usec_t ratelimit_ಕೊನೆ(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಆದರೆ (rl->begin == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ usec_ಕೂಡಿಸು(rl->begin, rl->interval);
}

usec_t ratelimit_ಎಡ(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl) {
        ಪ್ರತಿಜ್ಞಾ(rl);

        ಆದರೆ (rl->begin == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ usec_sub_unsigned(ratelimit_ಕೊನೆ(rl), now(CLOCK_BOOTTIME));
}
