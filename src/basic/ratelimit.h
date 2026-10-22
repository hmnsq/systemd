/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        unsigned burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        unsigned ଅଙ୍କ;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (ଅଚଳ RateLimit) { .interval = USEC_INFINITY, .burst = UINT_MAX }

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ratelimit_reset(RateLimit *rl) {
        rl->ଅଙ୍କ = rl->begin = 0;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ratelimit_configured(ଅଚଳ RateLimit *rl) {
        ଫେରନ୍ତୁ rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

unsigned ratelimit_ଅଙ୍କ_dropped(ଅଚଳ RateLimit *rl);

usec_t ratelimit_end(ଅଚଳ RateLimit *rl);
usec_t ratelimit_ବାମ(ଅଚଳ RateLimit *rl);
