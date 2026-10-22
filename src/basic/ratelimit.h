/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ದರ_ಎಲ್ಲೆ {
        usec_t interval; /* Keep those two fields first so they can be initialized easily: */
        ಚಿಹ್ನೆರಹಿತ ಅವಧಿ;  /* RateLimit rl = { INTERVAL, BURST }; */
        ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ;
        usec_t begin;
} ದರ_ಎಲ್ಲೆ;

#ವ್ಯಾಖ್ಯಾನಿಸು RATELIMIT_OFF (ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ) { .interval = USEC_INFINITY, .ಅವಧಿ = UINT_MAX }

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ratelimit_reset(ದರ_ಎಲ್ಲೆ *rl) {
        rl->ಅಂಕೆ = rl->begin = 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ratelimit_configured(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl) {
        ಹಿಂತಿರುಗಿ rl->interval > 0 && rl->ಅವಧಿ > 0;
}

bool ratelimit_ಕೆಳಗೆ(ದರ_ಎಲ್ಲೆ *rl);
/* Like ratelimit_below(), but takes the current CLOCK_BOOTTIME timestamp from the caller, for callers
 * that check many rate limits in a row and should not read the clock for each of them. The timestamps
 * passed for a given RateLimit must not go backwards: a ts before the one that opened the current
 * window makes usec_sub_unsigned() return 0, i.e. the window looks like it just started. */
bool ratelimit_ಕೆಳಗೆ_at(ದರ_ಎಲ್ಲೆ *rl, usec_t ts);

ಚಿಹ್ನೆರಹಿತ ratelimit_ಅಂಕೆ_dropped(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl);

usec_t ratelimit_ಕೊನೆ(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl);
usec_t ratelimit_ಎಡ(ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ *rl);
