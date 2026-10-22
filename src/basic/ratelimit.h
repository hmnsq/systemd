/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنية شرح_حد_بندی {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        بنانشانی مدت;  /* RateLimit rl = { INTERVAL, BURST }; */
        بنانشانی عدد;
        usec_t begin;
} شرح_حد_بندی;

#تعریف_کرو RATELIMIT_OFF (ثابت شرح_حد_بندی) { .interval = USEC_INFINITY, .مدت = UINT_MAX }

ساكن مضمن فراغ ratelimit_reset(شرح_حد_بندی *rl) {
        rl->عدد = rl->begin = 0;
}

ساكن مضمن bool ratelimit_configured(ثابت شرح_حد_بندی *rl) {
        أرجع rl->interval > 0 && rl->مدت > 0;
}

bool ratelimit_ہیٹھاں(شرح_حد_بندی *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_ہیٹھاں_at(شرح_حد_بندی *rl, usec_t ts);

بنانشانی ratelimit_عدد_dropped(ثابت شرح_حد_بندی *rl);

usec_t ratelimit_سرا(ثابت شرح_حد_بندی *rl);
usec_t ratelimit_بایاں(ثابت شرح_حد_بندی *rl);
