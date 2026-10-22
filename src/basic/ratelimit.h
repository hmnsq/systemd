/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന RateLimit {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        unsigned burst;  /* RateLimit rl = { INTERVAL, BURST }; */
        unsigned അക്കം;
        usec_t begin;
} RateLimit;

#define RATELIMIT_OFF (സ്ഥിരം RateLimit) { .interval = USEC_INFINITY, .burst = UINT_MAX }

സ്ഥിതം വരിയിൽ ശൂന്യം ratelimit_reset(RateLimit *rl) {
        rl->അക്കം = rl->begin = 0;
}

സ്ഥിതം വരിയിൽ bool ratelimit_configured(സ്ഥിരം RateLimit *rl) {
        തിരികെ rl->interval > 0 && rl->burst > 0;
}

bool ratelimit_below(RateLimit *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_below_at(RateLimit *rl, usec_t ts);

unsigned ratelimit_അക്കം_dropped(സ്ഥിരം RateLimit *rl);

usec_t ratelimit_end(സ്ഥിരം RateLimit *rl);
usec_t ratelimit_ഇടത്(സ്ഥിരം RateLimit *rl);
