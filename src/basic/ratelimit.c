/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "ratelimit.h"
#అన్తర్భావ "time-util.h"

/* Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2. */

bool ratelimit_క్రింద_at(రేటు_హద్దు *rl, usec_t ts) {
        ప్రతిజ్ఞా(rl);

        ఒకవేళ (!ratelimit_configured(rl))
                తిరిగి true;

        ఒకవేళ (rl->begin <= 0 ||
            usec_sub_unsigned(ts, rl->begin) > rl->interval) {
                rl->begin = ts;  /* Start a new time window */
                rl->అంకె = 1;     /* Reset counter */
                తిరిగి true;
        }

        ఒకవేళ (_unlikely_(rl->అంకె == UINT_MAX))
                తిరిగి false;

        rl->అంకె++;
        తిరిగి rl->అంకె <= rl->వ్యవధి;
}

bool ratelimit_క్రింద(రేటు_హద్దు *rl) {
        ప్రతిజ్ఞా(rl);

        ఒకవేళ (!ratelimit_configured(rl)) /* Keep this check here too: the argument below is evaluated
                                        * before the callee could short-circuit on it. */
                తిరిగి true;

        తిరిగి ratelimit_క్రింద_at(rl, now(CLOCK_BOOTTIME));
}

గుర్తులేని ratelimit_అంకె_dropped(స్థిరరాశి రేటు_హద్దు *rl) {
        ప్రతిజ్ఞా(rl);

        ఒకవేళ (rl->అంకె == UINT_MAX) /* overflow, return as special case */
                తిరిగి UINT_MAX;

        తిరిగి LESS_BY(rl->అంకె, rl->వ్యవధి);
}

usec_t ratelimit_చివర(స్థిరరాశి రేటు_హద్దు *rl) {
        ప్రతిజ్ఞా(rl);

        ఒకవేళ (rl->begin == 0)
                తిరిగి 0;

        తిరిగి usec_కూడు(rl->begin, rl->interval);
}

usec_t ratelimit_ఎడమ(స్థిరరాశి రేటు_హద్దు *rl) {
        ప్రతిజ్ఞా(rl);

        ఒకవేళ (rl->begin == 0)
                తిరిగి 0;

        తిరిగి usec_sub_unsigned(ratelimit_చివర(rl), now(CLOCK_BOOTTIME));
}
