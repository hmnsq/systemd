/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        अचिन्हित burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        अचिन्हित अङ्क;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (अचर RateLimit) { .interval = USEC_INFINITY, .burst = अच_पूर्णांक_उच्च }

स्थिर अन्तःस्थ शून्य ratelimit_reset(RateLimit *rl) {
        rl->अङ्क = rl->begin = 0;
}

स्थिर अन्तःस्थ bool ratelimit_configured(अचर RateLimit *rl) {
        फर्काऊ rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

अचिन्हित ratelimit_अङ्क_dropped(अचर RateLimit *rl);

usec_t ratelimit_end(अचर RateLimit *rl);
usec_t ratelimit_बायाँ(अचर RateLimit *rl);
