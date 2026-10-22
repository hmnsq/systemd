/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع بنية شرح_حد_بندي {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        بنانشان مدو;  /* RateLimit rl = { INTERVAL, BURST }; */
        بنانشان انگ;
        usec_t begin;
} شرح_حد_بندي;

#وصف_ڪريو RATELIMIT_OFF (ثابت شرح_حد_بندي) { .interval = USEC_INFINITY, .مدو = UINT_MAX }

ساكن مضمن فراغ ratelimit_reset(شرح_حد_بندي *rl) {
        rl->انگ = rl->begin = 0;
}

ساكن مضمن bool ratelimit_configured(ثابت شرح_حد_بندي *rl) {
        أرجع rl->interval > 0 && rl->مدو > 0;
}

bool ratelimit_هيٺ(شرح_حد_بندي *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_هيٺ_at(شرح_حد_بندي *rl, usec_t ts);

بنانشان ratelimit_انگ_dropped(ثابت شرح_حد_بندي *rl);

usec_t ratelimit_پڇاڙي(ثابت شرح_حد_بندي *rl);
usec_t ratelimit_کاٻو(ثابت شرح_حد_بندي *rl);
