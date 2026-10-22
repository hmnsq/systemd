/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_هيٺ_at(شرح_حد_بندي *rl, usec_t ts) {
        أكد(rl);

        إذا (!ratelimit_configured(rl))
                أرجع true;

        إذا (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->انگ = 1;     /* Reset counter */
                أرجع true;
        }

        إذا (_unlikely_(rl->انگ == UINT_MAX))
                أرجع false;

        rl->انگ++;
        أرجع rl->انگ <= rl->مدو;
}

bool ratelimit_هيٺ(شرح_حد_بندي *rl) {
        أكد(rl);

        إذا (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                أرجع true;

        أرجع ratelimit_هيٺ_at(rl, now(CLOCK_BOOTTIME));
}

بنانشان ratelimit_انگ_dropped(ثابت شرح_حد_بندي *rl) {
        أكد(rl);

        إذا (rl->انگ == UINT_MAX) /* overflow, return as special case */
                أرجع UINT_MAX;

        أرجع LESS_BY(rl->انگ, rl->مدو);
}

usec_t ratelimit_پڇاڙي(ثابت شرح_حد_بندي *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_جوڙيو(rl->begin, rl->interval);
}

usec_t ratelimit_کاٻو(ثابت شرح_حد_بندي *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_sub_unsigned(ratelimit_پڇاڙي(rl), now(CLOCK_BOOTTIME));
}
