/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        unsigned burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        unsigned અંક;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (અચલ RateLimit) { .interval = USEC_INFINITY, .burst = UINT_MAX }

સ્થિર અંતઃરેખ શૂન્ય ratelimit_reset(RateLimit *rl) {
        rl->અંક = rl->begin = 0;
}

સ્થિર અંતઃરેખ bool ratelimit_configured(અચલ RateLimit *rl) {
        પાછા rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

unsigned ratelimit_અંક_dropped(અચલ RateLimit *rl);

usec_t ratelimit_end(અચલ RateLimit *rl);
usec_t ratelimit_ડાબું(અચલ RateLimit *rl);
