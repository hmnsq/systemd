/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या संरचना RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        अचिन्हित burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        अचिन्हित अंक;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (स्थिरांक RateLimit) { .interval = USEC_INFINITY, .burst = अच_पूर्णांक_उच्च }

स्थिर ओळीत शून्य ratelimit_reset(RateLimit *rl) {
        rl->अंक = rl->begin = 0;
}

स्थिर ओळीत bool ratelimit_configured(स्थिरांक RateLimit *rl) {
        परत rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

अचिन्हित ratelimit_अंक_dropped(स्थिरांक RateLimit *rl);

usec_t ratelimit_end(स्थिरांक RateLimit *rl);
usec_t ratelimit_डावा(स्थिरांक RateLimit *rl);
