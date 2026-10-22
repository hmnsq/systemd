/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        unsigned burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        unsigned ಅಂಕೆ;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (ಧ್ರುವ RateLimit) { .interval = USEC_INFINITY, .burst = UINT_MAX }

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ ratelimit_reset(RateLimit *rl) {
        rl->ಅಂಕೆ = rl->begin = 0;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ratelimit_configured(ಧ್ರುವ RateLimit *rl) {
        ಪ್ರತ್ಯಾಗಮ rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

unsigned ratelimit_ಅಂಕೆ_dropped(ಧ್ರುವ RateLimit *rl);

usec_t ratelimit_end(ಧ್ರುವ RateLimit *rl);
usec_t ratelimit_ಎಡ(ಧ್ರುವ RateLimit *rl);
