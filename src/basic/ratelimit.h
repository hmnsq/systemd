/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना दरः_सीमा {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        अचिह्नित burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        अचिह्नित अङ्कः;
        usec_t begin;
} दरः_सीमा;

#परिभाषय RATELIMIT_OFF (ध्रुव दरः_सीमा) { .interval = USEC_INFINITY, .burst = UINT_MAX }

स्थिर अन्तःस्थ शून्य ratelimit_reset(दरः_सीमा *rl) {
        rl->अङ्कः = rl->begin = 0;
}

स्थिर अन्तःस्थ bool ratelimit_configured(ध्रुव दरः_सीमा *rl) {
        प्रत्यागम rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_अधः(दरः_सीमा *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_अधः_at(दरः_सीमा *rl, usec_t ts);

अचिह्नित ratelimit_अङ्कः_dropped(ध्रुव दरः_सीमा *rl);

usec_t ratelimit_अन्तः(ध्रुव दरः_सीमा *rl);
usec_t ratelimit_वामः(ध्रुव दरः_सीमा *rl);
