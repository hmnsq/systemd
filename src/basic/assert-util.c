/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>

#অন্তর্ভাৱ "assert-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
স্থির ৱর্ণ *log_abort_বাৰ্তা = রিক্ত;

স্থির শূন্য log_assert(
        পূর্ণাঙ্ক level,
        ধ্রুৱ ৱর্ণ *text,
        ধ্রুৱ ৱর্ণ *file,
        পূর্ণাঙ্ক সাৰি,
        ধ্রুৱ ৱর্ণ *func,
        ধ্রুৱ ৱর্ণ *format) {

        স্থির ৱর্ণ buffer[LINE_MAX];

        যদি (_likely_(LOG_PRI(level) > log_get_max_level()))
                ঘূৰাই;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (শূন্য) snprintf(buffer, পরিমাণ buffer, format, text, file, সাৰি, func);
        REENABLE_WARNING;

        log_abort_বাৰ্তা = buffer;

        log_dispatch_internal(level, 0, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত, buffer);
}

_noreturn_ শূন্য log_assert_failed(ধ্রুৱ ৱর্ণ *text, ধ্রুৱ ৱর্ণ *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *func) {
        log_assert(LOG_CRIT, text, file, সাৰি, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ শূন্য log_assert_failed_unreachable(ধ্রুৱ ৱর্ণ *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, সাৰি, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

শূন্য log_assert_failed_return(ধ্রুৱ ৱর্ণ *text, ধ্রুৱ ৱর্ণ *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        যদি (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, সাৰি, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, সাৰি, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
