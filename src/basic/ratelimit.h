/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع بنية شرح_حد {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        بینښه موده;  /* RateLimit rl = { INTERVAL, BURST }; */
        بینښه عدد;
        usec_t begin;
} شرح_حد;

#تعریف_کړه RATELIMIT_OFF (ثابت شرح_حد) { .interval = USEC_INFINITY, .موده = UINT_MAX }

ساكن مضمن فراغ ratelimit_reset(شرح_حد *rl) {
        rl->عدد = rl->begin = 0;
}

ساكن مضمن bool ratelimit_configured(ثابت شرح_حد *rl) {
        أرجع rl->interval > 0 && rl->موده > 0;
}

bool ratelimit_لاندې(شرح_حد *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_لاندې_at(شرح_حد *rl, usec_t ts);

بینښه ratelimit_عدد_dropped(ثابت شرح_حد *rl);

usec_t ratelimit_پای_ټکی(ثابت شرح_حد *rl);
usec_t ratelimit_کیڼ(ثابت شرح_حد *rl);
