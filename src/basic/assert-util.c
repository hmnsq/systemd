/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "assert-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ସ୍ଥିର ଅକ୍ଷର *log_abort_ସନ୍ଦେଶ = ଖାଲି;

ସ୍ଥିର ଶୂନ୍ୟ log_assert(
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
        ଅଚଳ ଅକ୍ଷର *text,
        ଅଚଳ ଅକ୍ଷର *file,
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
        ଅଚଳ ଅକ୍ଷର *func,
        ଅଚଳ ଅକ୍ଷର *format) {

        ସ୍ଥିର ଅକ୍ଷର buffer[LINE_MAX];

        ଯଦି (_likely_(LOG_PRI(level) > log_get_max_level()))
                ଫେରନ୍ତୁ;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (ଶୂନ୍ୟ) snprintf(buffer, ଆକାର buffer, format, text, file, ଧାଡ଼ି, func);
        REENABLE_WARNING;

        log_abort_ସନ୍ଦେଶ = buffer;

        log_dispatch_internal(level, 0, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, buffer);
}

_noreturn_ ଶୂନ୍ୟ log_assert_failed(ଅଚଳ ଅକ୍ଷର *text, ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *func) {
        log_assert(LOG_CRIT, text, file, ଧାଡ଼ି, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ ଶୂନ୍ୟ log_assert_failed_unreachable(ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, ଧାଡ଼ି, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

ଶୂନ୍ୟ log_assert_failed_return(ଅଚଳ ଅକ୍ଷର *text, ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        ଯଦି (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, ଧାଡ଼ି, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, ଧାଡ଼ି, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
