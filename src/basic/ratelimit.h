/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం రేటు_హద్దు {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        గుర్తులేని వ్యవధి;  /* RateLimit rl = { INTERVAL, BURST }; */
        గుర్తులేని అంకె;
        usec_t begin;
} రేటు_హద్దు;

#నిర్వచించు RATELIMIT_OFF (స్థిరరాశి రేటు_హద్దు) { .interval = USEC_INFINITY, .వ్యవధి = UINT_MAX }

స్థిర అంతఃవరుస శూన్య ratelimit_reset(రేటు_హద్దు *rl) {
        rl->అంకె = rl->begin = 0;
}

స్థిర అంతఃవరుస bool ratelimit_configured(స్థిరరాశి రేటు_హద్దు *rl) {
        తిరిగి rl->interval > 0 && rl->వ్యవధి > 0;
}

bool ratelimit_క్రింద(రేటు_హద్దు *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_క్రింద_at(రేటు_హద్దు *rl, usec_t ts);

గుర్తులేని ratelimit_అంకె_dropped(స్థిరరాశి రేటు_హద్దు *rl);

usec_t ratelimit_చివర(స్థిరరాశి రేటు_హద్దు *rl);
usec_t ratelimit_ఎడమ(స్థిరరాశి రేటు_హద్దు *rl);
