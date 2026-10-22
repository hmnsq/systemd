/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "assert-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
স্থির অক্ষর *log_abort_বার্তা = ফাঁকা;

স্থির শূন্য log_assert(
        পূর্ণসংখ্যা level,
        ধ্রুবক অক্ষর *text,
        ধ্রুবক অক্ষর *file,
        পূর্ণসংখ্যা সারি,
        ধ্রুবক অক্ষর *func,
        ধ্রুবক অক্ষর *format) {

        স্থির অক্ষর বাফার[LINE_MAX];

        যদি (_likely_(LOG_PRI(level) > log_get_max_level()))
                ফেরত;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (শূন্য) snprintf(বাফার, আকার বাফার, format, text, file, সারি, func);
        REENABLE_WARNING;

        log_abort_বার্তা = বাফার;

        log_dispatch_internal(level, 0, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, বাফার);
}

_noreturn_ শূন্য log_assert_failed(ধ্রুবক অক্ষর *text, ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *func) {
        log_assert(LOG_CRIT, text, file, সারি, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ শূন্য log_assert_failed_unreachable(ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, সারি, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

শূন্য log_assert_failed_return(ধ্রুবক অক্ষর *text, ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        যদি (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, সারি, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, সারি, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
