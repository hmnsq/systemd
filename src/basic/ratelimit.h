/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع بنية RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        unsigned burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        unsigned num;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (ثابت RateLimit) { .interval = USEC_INFINITY, .burst = UINT_MAX }

ساكن مضمن فراغ ratelimit_reset(RateLimit *rl) {
        rl->num = rl->begin = 0;
}

ساكن مضمن bool ratelimit_configured(ثابت RateLimit *rl) {
        החזר rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

unsigned ratelimit_num_dropped(ثابت RateLimit *rl);

usec_t ratelimit_end(ثابت RateLimit *rl);
usec_t ratelimit_left(ثابت RateLimit *rl);
