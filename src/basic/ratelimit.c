/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "ratelimit.h"
#شامل "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_نیچے_at(شرح_حد_بندی *rl, usec_t ts) {
        أكد(rl);

        اگر (!ratelimit_configured(rl))
                واپس true;

        اگر (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->عدد = 1;     /* Reset counter */
                واپس true;
        }

        اگر (_unlikely_(rl->عدد == UINT_MAX))
                واپس false;

        rl->عدد++;
        واپس rl->عدد <= rl->مدت;
}

bool ratelimit_نیچے(شرح_حد_بندی *rl) {
        أكد(rl);

        اگر (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                واپس true;

        واپس ratelimit_نیچے_at(rl, now(CLOCK_BOOTTIME));
}

غیرعلامتی ratelimit_عدد_dropped(ثابت شرح_حد_بندی *rl) {
        أكد(rl);

        اگر (rl->عدد == UINT_MAX) /* overflow, return as special case */
                واپس UINT_MAX;

        واپس LESS_BY(rl->عدد, rl->مدت);
}

usec_t ratelimit_انتہا(ثابت شرح_حد_بندی *rl) {
        أكد(rl);

        اگر (rl->begin == 0)
                واپس 0;

        واپس usec_جوڑو(rl->begin, rl->interval);
}

usec_t ratelimit_بایاں(ثابت شرح_حد_بندی *rl) {
        أكد(rl);

        اگر (rl->begin == 0)
                واپس 0;

        واپس usec_sub_unsigned(ratelimit_انتہا(rl), now(CLOCK_BOOTTIME));
}
