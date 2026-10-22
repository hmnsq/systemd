/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع بنية نرخ_حد {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        بیعلامت مدت;  /* RateLimit rl = { INTERVAL, BURST }; */
        بیعلامت رقم;
        usec_t begin;
} نرخ_حد;

#تعریف_کن RATELIMIT_OFF (ثابت نرخ_حد) { .interval = USEC_INFINITY, .مدت = UINT_MAX }

ساكن مضمن فراغ ratelimit_reset(نرخ_حد *rl) {
        rl->رقم = rl->begin = 0;
}

ساكن مضمن bool ratelimit_configured(ثابت نرخ_حد *rl) {
        أرجع rl->interval > 0 && rl->مدت > 0;
}

bool ratelimit_زیر(نرخ_حد *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_زیر_at(نرخ_حد *rl, usec_t ts);

بیعلامت ratelimit_رقم_dropped(ثابت نرخ_حد *rl);

usec_t ratelimit_انتها(ثابت نرخ_حد *rl);
usec_t ratelimit_چپ(ثابت نرخ_حد *rl);
