/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "assert-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
స్థిర అక్షరం *log_abort_సందేశం = ఖాళీ;

స్థిర శూన్య log_assert(
        పూర్ణాంకం level,
        స్థిరరాశి అక్షరం *పాఠ్యం,
        స్థిరరాశి అక్షరం *file,
        పూర్ణాంకం పంక్తి,
        స్థిరరాశి అక్షరం *func,
        స్థిరరాశి అక్షరం *format) {

        స్థిర అక్షరం బఫరు[LINE_MAX];

        ఒకవేళ (_likely_(LOG_PRI(level) > log_get_max_level()))
                తిరిగి;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (శూన్య) snprintf(బఫరు, పరిమాణం బఫరు, format, పాఠ్యం, file, పంక్తి, func);
        REENABLE_WARNING;

        log_abort_సందేశం = బఫరు;

        log_dispatch_అంతర్గత(level, 0, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, బఫరు);
}

_noreturn_ శూన్య log_assert_failed(స్థిరరాశి అక్షరం *పాఠ్యం, స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *func) {
        log_assert(LOG_CRIT, పాఠ్యం, file, పంక్తి, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ శూన్య log_assert_failed_unreachable(స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, పంక్తి, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

శూన్య log_assert_failed_return(స్థిరరాశి అక్షరం *పాఠ్యం, స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        ఒకవేళ (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(పాఠ్యం, file, పంక్తి, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, పాఠ్యం, file, పంక్తి, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
