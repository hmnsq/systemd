/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "ratelimit.h"
#ضمن "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_زیر_at(نرخ_حد *rl, usec_t ts) {
        أكد(rl);

        إذا (!ratelimit_configured(rl))
                أرجع true;

        إذا (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->رقم = 1;     /* Reset counter */
                أرجع true;
        }

        إذا (_unlikely_(rl->رقم == UINT_MAX))
                أرجع false;

        rl->رقم++;
        أرجع rl->رقم <= rl->مدت;
}

bool ratelimit_زیر(نرخ_حد *rl) {
        أكد(rl);

        إذا (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                أرجع true;

        أرجع ratelimit_زیر_at(rl, now(CLOCK_BOOTTIME));
}

بیعلامت ratelimit_رقم_dropped(ثابت نرخ_حد *rl) {
        أكد(rl);

        إذا (rl->رقم == UINT_MAX) /* overflow, return as special case */
                أرجع UINT_MAX;

        أرجع LESS_BY(rl->رقم, rl->مدت);
}

usec_t ratelimit_انتها(ثابت نرخ_حد *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_جمع_کن(rl->begin, rl->interval);
}

usec_t ratelimit_چپ(ثابت نرخ_حد *rl) {
        أكد(rl);

        إذا (rl->begin == 0)
                أرجع 0;

        أرجع usec_sub_unsigned(ratelimit_انتها(rl), now(CLOCK_BOOTTIME));
}
