/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

స్థిరరాశి అక్షరం* errno_name_no_fallback(పూర్ణాంకం id) _const_;
పూర్ణాంకం errno_from_name(స్థిరరాశి అక్షరం *name) శుద్ధ;

స్థిర అంతఃవరుస bool errno_is_valid(పూర్ణాంకం n) {
        తిరిగి n > 0 && n <= ERRNO_MAX;
}

#నిర్వచించు ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(పూర్ణాంకం)
/* Like errno_name, but always returns a string. */
స్థిరరాశి అక్షరం* errno_name(పూర్ణాంకం id, అక్షరం buf[స్థిర ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#నిర్వచించు ERRNO_NAME(errnum) errno_name(errnum, (అక్షరం[ERRNO_NAME_BUF_LEN]){})
