/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "ratelimit.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_ਹੇਠਾਂ_at(ਦਰ_ਹੱਦਬੰਦੀ *rl, usec_t ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(rl);

        ਜੇ (!ratelimit_configured(rl))
                ਵਾਪਸ true;

        ਜੇ (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->ਅੰਕ = 1;     /* Reset counter */
                ਵਾਪਸ true;
        }

        ਜੇ (_unlikely_(rl->ਅੰਕ == UINT_MAX))
                ਵਾਪਸ false;

        rl->ਅੰਕ++;
        ਵਾਪਸ rl->ਅੰਕ <= rl->ਮਿਆਦ;
}

bool ratelimit_ਹੇਠਾਂ(ਦਰ_ਹੱਦਬੰਦੀ *rl) {
        ਪ੍ਰਤਿਜ੍ਞਾ(rl);

        ਜੇ (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                ਵਾਪਸ true;

        ਵਾਪਸ ratelimit_ਹੇਠਾਂ_at(rl, now(CLOCK_BOOTTIME));
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ratelimit_ਅੰਕ_dropped(ਅਚਲ ਦਰ_ਹੱਦਬੰਦੀ *rl) {
        ਪ੍ਰਤਿਜ੍ਞਾ(rl);

        ਜੇ (rl->ਅੰਕ == UINT_MAX) /* overflow, return as special case */
                ਵਾਪਸ UINT_MAX;

        ਵਾਪਸ LESS_BY(rl->ਅੰਕ, rl->ਮਿਆਦ);
}

usec_t ratelimit_ਸਿਰਾ(ਅਚਲ ਦਰ_ਹੱਦਬੰਦੀ *rl) {
        ਪ੍ਰਤਿਜ੍ਞਾ(rl);

        ਜੇ (rl->begin == 0)
                ਵਾਪਸ 0;

        ਵਾਪਸ usec_ਜੋੜੋ(rl->begin, rl->interval);
}

usec_t ratelimit_ਖੱਬਾ(ਅਚਲ ਦਰ_ਹੱਦਬੰਦੀ *rl) {
        ਪ੍ਰਤਿਜ੍ਞਾ(rl);

        ਜੇ (rl->begin == 0)
                ਵਾਪਸ 0;

        ਵਾਪਸ usec_sub_unsigned(ratelimit_ਸਿਰਾ(rl), now(CLOCK_BOOTTIME));
}
