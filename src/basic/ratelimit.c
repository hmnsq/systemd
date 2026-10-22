/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_ہیٹھاں_at(شرح_حد_بندی *rl, usec_t ts) {
        أكد(rl);

        إذا (!ratelimit_configured(rl))
                أرجع true;

        إذا (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->عدد = 1;     /* Reset counter */
                أرجع true;
        }

        إذا (_unlikely_(rl->عدد == UINT_MAX))
                أرجع false;

        rl->عدد++;
        أرجع rl->عدد <= rl->مدت;
}

bool ratelimit_ہیٹھاں(شرح_حد_بندی *rl) {
        أكد(rl);

        إذا (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                أرجع true;

        أرجع ratelimit_ہیٹھاں_at(rl, now(CLOCK_BOOTTIME));
}

بنانشانی ratelimit_عدد_dropped(ثابت شرح_حد_بندی *rl) {
        أكد(rl);

        إذا (rl->عدد == UINT_MAX) /* overflow, return as special case */
                أرجع UINT_MAX;

        أرجع LESS_BY(rl->عدد, rl->مدت);
}

usec_t ratelimit_سرا(ثابت شرح_حد_بندی *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_جوڑو(rl->begin, rl->interval);
}

usec_t ratelimit_بایاں(ثابت شرح_حد_بندی *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_sub_unsigned(ratelimit_سرا(rl), now(CLOCK_BOOTTIME));
}
